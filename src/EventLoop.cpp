#include "EventLoop.h"

int createTimerfd(int sec = 30)
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    howlong.it_value.tv_nsec = 0;
    timerfd_settime(timerfd, 0, &howlong, 0);
    return timerfd;
}

EventLoop::EventLoop(bool mainloop, int timetvl, int timeout) : epoll_(new Epoll()),
                                                                wakeupFd_(eventfd(0, EFD_NONBLOCK)),
                                                                wakeupChannel_(new Channel(this, wakeupFd_)),
                                                                timerFd_(createTimerfd(timetvl)),
                                                                timerChannel_(new Channel(this, timerFd_)),
                                                                mainloop_(mainloop),
                                                                timetvl_(timetvl),
                                                                timeout_(timeout),
                                                                quit_(false)
{
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleWakeUp, this));
    wakeupChannel_->enableReading();

    timerChannel_->setReadCallback(std::bind(&EventLoop::handleTimeout, this));
    timerChannel_->enableReading();
}

EventLoop::~EventLoop()
{
}

void EventLoop::run()
{
    threadId_ = syscall(SYS_gettid); // 获取当前线程的tid。
    while (!quit_)                   // 事件循环。
    {
        vector<Channel *> channels = epoll_->loop(); // epoll_wait()，阻塞等待事件发生。
        if (channels.empty() && timeoutCallback_)
        {
            timeoutCallback_(this);
        }
        // 如果infds>0，表示有事件发生的fd的数量。
        for (auto &channel : channels) // 遍历epoll返回的数组evs。
        {
            channel->handleEvent(); // 处理事件。
        }
    }
}

void EventLoop::stop()
{
    quit_ = true;
    wakeup();
}

void EventLoop::updateChannel(Channel *channel)
{
    epoll_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    epoll_->removeChannel(channel);
}

void EventLoop::setTimeoutCallback(const std::function<void(EventLoop *)> &cb)
{
    timeoutCallback_ = cb;
}

bool EventLoop::isInLoopThread() const
{
    return threadId_ == syscall(SYS_gettid);
}

void EventLoop::queueInLoop(const std::function<void()> &cb)
{
    {
        std::lock_guard<std::mutex> lock(taskqueuMutex_);
        taskqueu_.push(cb);
    }
    // 唤醒一个线程。
    wakeup();
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        printf("EventLoop::wakeup() writes %ld bytes instead of 8\n", n);
    }
}

void EventLoop::handleWakeUp()
{
    printf("EventLoop::handleWakeUp(),thread id:%ld\n", syscall(SYS_gettid));
    uint64_t one;
    read(wakeupFd_, &one, sizeof(one));

    std::function<void()> task;

    std::lock_guard<std::mutex> lock(taskqueuMutex_);
    while (taskqueu_.size() > 0)
    {
        task = std::move(taskqueu_.front());
        taskqueu_.pop();
        task();
    }
}

void EventLoop::handleTimeout()
{

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = timetvl_;
    howlong.it_value.tv_nsec = 0;
    timerfd_settime(timerFd_, 0, &howlong, 0);
    if (mainloop_)
    {
    }
    else
    {
        time_t now = time(0); // 获取当前时间。
        std::vector<int> keysToDelete;

        for (auto aa : connMap_)
        {
            if (aa.second->timeout(now, timeout_))
            {
                keysToDelete.push_back(aa.first); // 收集需要删除的键
            }
        }

        // 删除操作
        for (auto key : keysToDelete)
        {
            std::lock_guard<std::mutex> gd(connMutex_);
            connMap_.erase(key);
            timercallback_(key);
        }
    }
}

void EventLoop::addConnection(spConnection conn)
{
    std::lock_guard<std::mutex> gd(connMutex_);
    connMap_[conn->getFd()] = conn;
}

void EventLoop::settimercallback(std::function<void(int)> fn)
{
    timercallback_ = fn;
}
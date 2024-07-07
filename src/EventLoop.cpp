#include "EventLoop.h"

EventLoop::EventLoop(/* args */) : epoll_(new Epoll()), wakeupFd_(eventfd(0, EFD_NONBLOCK)), wakeupChannel_(new Channel(this, wakeupFd_))
{
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleWakeUp, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
}

void EventLoop::run()
{
    threadId_ = syscall(SYS_gettid); // 获取当前线程的tid。
    while (true)                     // 事件循环。
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
        std::lock_guard<std::mutex> lock(mutex_);
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

    std::lock_guard<std::mutex> lock(mutex_);
    while (taskqueu_.size() > 0)
    {
        task = std::move(taskqueu_.front());
        taskqueu_.pop();
        task();
    }
}
#include "EventLoop.h"
#include <sys/syscall.h>

EventLoop::EventLoop(/* args */)
{
    epoll_ = new Epoll();
}

EventLoop::~EventLoop()
{
    delete epoll_;
}

void EventLoop::run()
{
    while (true) // 事件循环。
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

void EventLoop::setTimeoutCallback(const std::function<void(EventLoop *)> &cb)
{
    timeoutCallback_ = cb;
}
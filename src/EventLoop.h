#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include <memory>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <queue>
#include <mutex>

#include "Epoll.h"
#include "Channel.h"

class Channel;
class Epoll;
class EventLoop
{
private:
    /* data */
    std::unique_ptr<Epoll> epoll_;

    std::function<void(EventLoop *)> timeoutCallback_;

    pid_t threadId_;

    queue<std::function<void()>> taskqueu_;
    std::mutex mutex_;
    int wakeupFd_;

    std::unique_ptr<Channel> wakeupChannel_;

public:
    EventLoop(/* args */);
    ~EventLoop();

    void run();
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

    void setTimeoutCallback(const std::function<void(EventLoop *)> &cb);

    bool isInLoopThread() const;

    void queueInLoop(const std::function<void()> &cb);
    void wakeup();
    void handleWakeUp();
};

#endif
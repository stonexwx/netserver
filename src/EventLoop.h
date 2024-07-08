#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include <memory>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <queue>
#include <mutex>
#include <map>
#include <atomic>

#include "Epoll.h"
#include "Channel.h"
#include "Connection.h"

class Channel;
class Epoll;
class Connection;

using spConnection = std::shared_ptr<Connection>;

class EventLoop
{
private:
    /* data */
    std::unique_ptr<Epoll> epoll_;

    std::function<void(EventLoop *)> timeoutCallback_;
    std::function<void(int)> timercallback_;

    pid_t threadId_;

    queue<std::function<void()>> taskqueu_;
    std::mutex taskqueuMutex_;

    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    int timerFd_;
    std::unique_ptr<Channel> timerChannel_;
    bool mainloop_;

    std::map<int, spConnection> connMap_;
    std::mutex connMutex_;

    std::atomic_bool quit_;

    int timetvl_;
    int timeout_;

public:
    EventLoop(bool mainloop, int timetvl = 30, int timeout = 60);
    ~EventLoop();

    void run();
    void stop();

    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

    void setTimeoutCallback(const std::function<void(EventLoop *)> &cb);

    bool isInLoopThread() const;

    void queueInLoop(const std::function<void()> &cb);
    void wakeup();
    void handleWakeUp();

    void handleTimeout();

    void addConnection(spConnection conn);
    void settimercallback(std::function<void(int)> fn);
};

#endif
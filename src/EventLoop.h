#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include "Epoll.h"
#include "Channel.h"

class Channel;
class Epoll;
class EventLoop
{
private:
    /* data */
    Epoll *epoll_;

    std::function<void(EventLoop *)> timeoutCallback_;

public:
    EventLoop(/* args */);
    ~EventLoop();

    void run();
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

    void setTimeoutCallback(const std::function<void(EventLoop *)> &cb);
};

#endif
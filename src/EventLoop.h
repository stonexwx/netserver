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

public:
    EventLoop(/* args */);
    ~EventLoop();

    void run();
    void updateChannel(Channel *channel);
};

#endif
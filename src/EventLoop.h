#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include "Epoll.h"

class EventLoop
{
private:
    /* data */
    Epoll *epoll_;

public:
    EventLoop(/* args */);
    ~EventLoop();

    void run();
    Epoll *getEpoll() const;
};

#endif
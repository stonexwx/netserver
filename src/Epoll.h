#ifndef _EPOLL_H
#define _EPOLL_H

#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>

#include <string.h>

#include "Channel.h"

using namespace std;

class Channel;

class Epoll
{
private:
    /* data */
    static const int MAX_EVENTS = 100;
    int epoll_fd;
    struct epoll_event events_[MAX_EVENTS];

public:
    Epoll();
    ~Epoll();

    void addfd(int fd, uint32_t op);
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    // vector<struct epoll_event> loop(int timeout = -1);
    vector<Channel *> loop(int timeout = -1);
};

#endif
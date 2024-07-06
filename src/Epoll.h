#ifndef _EPOLL_H
#define _EPOLL_H

#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>

#include <string.h>

using namespace std;

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
    vector<struct epoll_event> loop(int timeout = -1);
};

#endif
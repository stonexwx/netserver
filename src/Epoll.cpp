#include "Epoll.h"

Epoll::Epoll()
{
    epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
    {
        printf("%s:%s:%d,err:%s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        exit(1);
    }
}

Epoll::~Epoll()
{
    close(epoll_fd);
    epoll_fd = -1;
    memset(events_, 0, sizeof(events_));
}

void Epoll::addfd(int fd, uint32_t op)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = op;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        printf("%s:%s:%d,err:%s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        exit(1);
    }
}

void Epoll::updateChannel(Channel *channel)
{
    struct epoll_event ev;
    ev.data.ptr = channel;
    ev.events = channel->getEvents();
    if (channel->inpoll())
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, channel->getFd(), &ev) == -1)
        {
            printf("%s:%s:%d,err:%s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            exit(1);
        }
    }
    else
    {
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, channel->getFd(), &ev) == -1)
        {
            printf("%s:%s:%d,err:%s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
            exit(1);
        }
        channel->setInEpoll();
    }
}

vector<Channel *> Epoll::loop(int timeout)
{
    vector<Channel *> ret;
    bzero(events_, sizeof(events_));
    int nfds = epoll_wait(epoll_fd, events_, MAX_EVENTS, timeout);
    if (nfds < 0)
    {
        printf("%s:%s:%d,err:%s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        exit(1);
    }
    if (nfds == 0)
    {
        return ret;
    }
    for (int i = 0; i < nfds; i++)
    {
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->setrevents(events_[i].events);
        ret.push_back(channel);
    }
    return ret;
}
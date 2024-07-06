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

vector<struct epoll_event> Epoll::loop(int timeout)
{
    vector<struct epoll_event> ret;
    bzero(events_, sizeof(events_));
    int nfds = epoll_wait(epoll_fd, events_, MAX_EVENTS, timeout);
    if (nfds < 0)
    {
        printf("%s:%s:%d,err:%s\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        exit(1);
    }
    if (nfds == 0)
    {
        printf("epoll_wait() timeout.\n");
        return ret;
    }
    for (int i = 0; i < nfds; i++)
    {
        ret.push_back(events_[i]);
    }
    return ret;
}
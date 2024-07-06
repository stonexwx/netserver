#include "Channel.h"

Channel::Channel(Epoll *ep, int fd) : ep_(ep), fd_(fd)
{
}

Channel::~Channel()
{
}

int Channel::getFd() const
{
    return fd_;
}

void Channel::useET()
{
    events_ |= EPOLLET;
}

void Channel::enableReading()
{
    events_ |= EPOLLIN;
    ep_->updateChannel(this);
}

void Channel::setInEpoll()
{
    inepoll_ = true;
}

void Channel::setrevents(uint32_t revents)
{
    revents_ = revents;
}

bool Channel::inpoll() const
{
    return inepoll_;
}

uint32_t Channel::getEvents() const
{
    return events_;
}

uint32_t Channel::getRevents() const
{
    return revents_;
}

#include "Channel.h"

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd)
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
    loop_->updateChannel(this);
}

void Channel::disableReading()
{
    events_ &= ~EPOLLIN;
    loop_->updateChannel(this);
}

void Channel::enableWriting()
{
    events_ |= EPOLLOUT;
    loop_->updateChannel(this);
}

void Channel::disableWriting()
{
    events_ &= ~EPOLLOUT;
    loop_->updateChannel(this);
}

void Channel::disableAllEvent()
{
    events_ = 0;
    loop_->updateChannel(this);
}

void Channel::remove()
{
    disableAllEvent();
    loop_->removeChannel(this);
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

void Channel::handleEvent()
{

    if (getRevents() & EPOLLRDHUP) // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
    {
        remove();
        closeCallback_();
    } //  普通数据  带外数据
    else if (getRevents() & (EPOLLIN | EPOLLPRI)) // 接收缓冲区中有数据可以读。
    {
        readCallback_();
    }
    else if (getRevents() & EPOLLOUT) // 有数据需要写，暂时没有代码，以后再说。
    {
        writeCallback_();
    }
    else // 其它事件，都视为错误。
    {
        remove();
        errorCallback_();
    }
}

void Channel::setReadCallback(const function<void()> &cb)
{
    readCallback_ = cb;
}

void Channel::setCloseCallback(const function<void()> &cb)
{
    closeCallback_ = cb;
}

void Channel::setErrorCallback(const function<void()> &cb)
{
    errorCallback_ = cb;
}

void Channel::setWriteCallback(const function<void()> &cb)
{
    writeCallback_ = cb;
}
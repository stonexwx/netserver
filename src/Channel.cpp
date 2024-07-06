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
        closeCallback_();
    } //  普通数据  带外数据
    else if (getRevents() & (EPOLLIN | EPOLLPRI)) // 接收缓冲区中有数据可以读。
    {
        readCallback_();
    }
    else if (getRevents() & EPOLLOUT) // 有数据需要写，暂时没有代码，以后再说。
    {
    }
    else // 其它事件，都视为错误。
    {
        errorCallback_();
    }
}

void Channel::handleRead()
{
    char buffer[1024];
    while (true) // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd_, buffer, sizeof(buffer)); // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
        if (nread > 0)                                     // 成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            printf("recv(eventfd=%d):%s\n", fd_, buffer);
            send(fd_, buffer, strlen(buffer), 0);
        }
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            break;
        }
        else if (nread == 0) // 客户端连接已断开。
        {
            closeCallback_();
            break;
        }
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
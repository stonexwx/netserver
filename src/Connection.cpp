#include "Connection.h"
#include <sys/syscall.h>
Connection::Connection(EventLoop *loop, Socket *clientSocket)
    : loop_(loop), clientSocket_(clientSocket)
{
    clientChannel_ = new Channel(loop_, clientSocket_->getFd());
    clientChannel_->enableReading();
    clientChannel_->useET(); // 使用ET模式。
    clientChannel_->setReadCallback(std::bind(&Connection::onMessageCallback, this));
    clientChannel_->setCloseCallback(std::bind(&Connection::closeCallback, this));
    clientChannel_->setErrorCallback(std::bind(&Connection::errorCallback, this));
    clientChannel_->setWriteCallback(std::bind(&Connection::writeCallback, this));
}

Connection::~Connection()
{
    delete clientChannel_;
    delete clientSocket_;
}

string Connection::getClientIp() const
{
    return clientSocket_->getIp();
}

uint16_t Connection::getClientPort() const
{
    return clientSocket_->getPort();
}

int Connection::getFd() const
{
    return clientSocket_->getFd();
}

void Connection::send(const char *data, size_t size)
{
    outputBuffer_.appendWithHead(data, size);
    clientChannel_->enableWriting();
}

void Connection::closeCallback()
{
    printf("client(eventfd=%d) disconnected.\n", getFd());
    close(getFd());
    closeCallback_(this);
}

void Connection::onMessageCallback()
{
    char buffer[1024];
    while (true) // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(getFd(), buffer, sizeof(buffer)); // 这行代码用了read()，也可以用recv()，一样的，不要纠结。

        if (nread > 0) // 成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            // printf("recv(eventfd=%d):%s\n", getFd(), buffer);
            // send(getFd(), buffer, strlen(buffer), 0);

            inputBuffer_.append(buffer, nread);
        }
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            // printf("recv(eventfd=%d): %s\n", getFd(), inputBuffer_.data());
            printf("threadid=%ld\n", syscall(SYS_gettid));
            while (true)
            {
                int len;
                memcpy(&len, inputBuffer_.data(), 4);
                if (inputBuffer_.size() < len + 4)
                {
                    break;
                }
                string message(inputBuffer_.data() + 4, len);
                inputBuffer_.erase(0, len + 4);
                printf("message(eventfd=%d): %s\n", getFd(), message.c_str());

                onMessageCallback_(this, message);
            }

            break;
        }
        else if (nread == 0) // 客户端连接已断开。
        {
            closeCallback(); // 关闭回调函数
            break;
        }
    }
}

void Connection::errorCallback()
{
    printf("client(eventfd=%d) error.\n", getFd());
    close(getFd());
    errorCallback_(this);
}

void Connection::writeCallback()
{
    if (outputBuffer_.size() > 0)
    {
        ssize_t nwrite = ::send(getFd(), outputBuffer_.data(), outputBuffer_.size(), 0);
        if (nwrite > 0)
        {
            outputBuffer_.erase(0, nwrite);
        }

        if (outputBuffer_.size() == 0)
        {
            clientChannel_->disableWriting();
            writeCompleteCallback_(this);
        }
    }
    else
    {
        clientChannel_->disableWriting();
        writeCompleteCallback_(this);
    }
}

void Connection::setCloseCallback(const std::function<void(Connection *)> &cb)
{
    closeCallback_ = cb;
}

void Connection::setErrorCallback(const std::function<void(Connection *)> &cb)
{
    errorCallback_ = cb;
}

void Connection::setOnMessageCallback(const std::function<void(Connection *, string &)> &cb)
{
    onMessageCallback_ = cb;
}

void Connection::setWriteCompleteCallback(const std::function<void(Connection *)> &cb)
{
    writeCompleteCallback_ = cb;
}

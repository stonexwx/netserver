#include "Connection.h"

Connection::Connection(EventLoop *loop, std::unique_ptr<Socket> clientSocket)
    : loop_(loop), clientSocket_(std::move(clientSocket)), disconnected_(false), clientChannel_(new Channel(loop_, clientSocket_->getFd()))
{
    clientChannel_->enableReading();
    clientChannel_->useET(); // 使用ET模式。
    clientChannel_->setReadCallback(std::bind(&Connection::onMessageCallback, this));
    clientChannel_->setCloseCallback(std::bind(&Connection::closeCallback, this));
    clientChannel_->setErrorCallback(std::bind(&Connection::errorCallback, this));
    clientChannel_->setWriteCallback(std::bind(&Connection::writeCallback, this));
}

Connection::~Connection()
{
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
    if (disconnected_)
    {
        return;
    }

    if (loop_->isInLoopThread()) // 如果在IO线程中，直接发送。
    {
        sendin(string(data), size);
    }
    else
    {

        loop_->queueInLoop(std::bind(&Connection::sendin, this, string(data), size));
    }
}
void Connection::sendin(const string data, size_t size)
{
    printf("sendin data:%s\n", data.c_str());
    outputBuffer_.appendWithHead(data.c_str(), size);
    clientChannel_->enableWriting();
}

bool Connection::timeout(time_t now, int seconds)
{
    return now - lastReceiveTime_.toint() > seconds;
}

void Connection::closeCallback()
{
    disconnected_ = true;
    clientChannel_->remove();
    closeCallback_(shared_from_this());
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
            inputBuffer_.append(buffer, nread);
        }
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            // printf("recv(eventfd=%d): %s\n", getFd(), inputBuffer_.data());
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
                lastReceiveTime_ = Timestamp::now();
                onMessageCallback_(shared_from_this(), message);
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
    disconnected_ = true;
    clientChannel_->remove();
    errorCallback_(shared_from_this());
}

void Connection::writeCallback()
{

    ssize_t nwrite = ::send(getFd(), outputBuffer_.data(), outputBuffer_.size(), 0);
    if (nwrite > 0)
    {
        outputBuffer_.erase(0, nwrite);
    }

    if (outputBuffer_.size() == 0)
    {

        clientChannel_->disableWriting();
        writeCompleteCallback_(shared_from_this());
    }
}

void Connection::setCloseCallback(const std::function<void(spConnection)> &cb)
{
    closeCallback_ = cb;
}

void Connection::setErrorCallback(const std::function<void(spConnection)> &cb)
{
    errorCallback_ = cb;
}

void Connection::setOnMessageCallback(const std::function<void(spConnection, string &)> &cb)
{
    onMessageCallback_ = cb;
}

void Connection::setWriteCompleteCallback(const std::function<void(spConnection)> &cb)
{
    writeCompleteCallback_ = cb;
}

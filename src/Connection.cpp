#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientSocket)
    : loop_(loop), clientSocket_(clientSocket)
{
    clientChannel_ = new Channel(loop_, clientSocket_->getFd());
    clientChannel_->enableReading();
    clientChannel_->useET(); // 使用ET模式。
    clientChannel_->setReadCallback(std::bind(&Channel::handleRead, clientChannel_));
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

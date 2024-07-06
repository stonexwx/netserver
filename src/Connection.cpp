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

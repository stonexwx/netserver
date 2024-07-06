#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop,
                   const string &ip,
                   const string &port)
    : loop_(loop)
{
    // 初始化acceptSocket_
    acceptSocket_ = new Socket(createNonblockingOrDie());

    InetAddress servaddr(ip, atoi(port.c_str()));
    acceptSocket_->setKeepAlive(true);
    acceptSocket_->setReuseAddr(true);
    acceptSocket_->setReusePort(true);
    acceptSocket_->setNoDelay(true);

    acceptSocket_->bindAddress(servaddr);
    acceptSocket_->listen();

    acceptChannel_ = new Channel(loop_, acceptSocket_->getFd());
    acceptChannel_->setReadCallback(std::bind(&Channel::newConnection, acceptChannel_, acceptSocket_));
    acceptChannel_->enableReading();
}

Acceptor::~Acceptor()
{
    delete acceptSocket_;
    delete acceptChannel_;
}
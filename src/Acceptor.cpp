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
    acceptChannel_->setReadCallback(std::bind(&Acceptor::newConnection, this));
    acceptChannel_->enableReading();
}

Acceptor::~Acceptor()
{
    delete acceptSocket_;
    delete acceptChannel_;
}

void Acceptor::newConnection()
{
    InetAddress clientaddr;
    Socket *clientfd = new Socket(acceptSocket_->accept(clientaddr)); // 接受新的客户端连接。

    printf("accept client(fd=%d,ip=%s,port=%d) ok.\n", clientfd->getFd(), clientaddr.getIp(), clientaddr.getPort());

    // 为新客户端连接准备读事件，并添加到epoll中。
    Connection *conn = new Connection(loop_, clientfd);
}
#include "TcpServer.h"

TcpServer::TcpServer(const string &ip, const string &port)
{
    // 初始化socket
    Socket *socket = new Socket(createNonblockingOrDie());

    InetAddress servaddr(ip, atoi(port.c_str()));
    socket->setKeepAlive(true);
    socket->setReuseAddr(true);
    socket->setReusePort(true);
    socket->setNoDelay(true);

    socket->bindAddress(servaddr);
    socket->listen();
    printf("fd:%d\n", socket->getFd());
    Channel *serveChannel = new Channel(&loop_, socket->getFd());
    serveChannel->setReadCallback(std::bind(&Channel::newConnection, serveChannel, socket));
    serveChannel->enableReading();
}

TcpServer::~TcpServer()
{
}

void TcpServer::tcpServerStart()
{
    loop_.run();
}

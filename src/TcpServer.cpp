#include "TcpServer.h"

TcpServer::TcpServer(const string &ip, const string &port)
{
    acceptor_ = new Acceptor(&loop_, ip, port);
}

TcpServer::~TcpServer()
{
    delete acceptor_;
}

void TcpServer::tcpServerStart()
{
    loop_.run();
}

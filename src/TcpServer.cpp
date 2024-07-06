#include "TcpServer.h"

TcpServer::TcpServer(const string &ip, const string &port)
{
    acceptor_ = new Acceptor(&loop_, ip, port);
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;
    for (auto iter = connMap_.begin(); iter != connMap_.end(); ++iter)
    {
        delete iter->second;
    }
}

void TcpServer::tcpServerStart()
{
    loop_.run();
}

void TcpServer::newConnection(Socket *clientSocket)
{
    Connection *conn_ = new Connection(&loop_, clientSocket);
    printf("accept client(fd=%d,ip=%s,port=%d) ok.\n",
           conn_->getFd(),
           conn_->getClientIp().c_str(),
           conn_->getClientPort());
    connMap_[conn_->getFd()] = conn_;
}
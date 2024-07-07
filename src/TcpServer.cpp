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

    conn_->setCloseCallback(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
    conn_->setErrorCallback(std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
    conn_->setOnMessageCallback(std::bind(&TcpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    connMap_[conn_->getFd()] = conn_;
}

void TcpServer::closeConnection(Connection *conn)
{
    auto iter = connMap_.find(conn->getFd());
    if (iter != connMap_.end())
    {
        delete iter->second;
        connMap_.erase(iter);
    }
}

void TcpServer::errorConnection(Connection *conn)
{
    auto iter = connMap_.find(conn->getFd());
    if (iter != connMap_.end())
    {
        delete iter->second;
        connMap_.erase(iter);
    }
}

void TcpServer::onMessage(Connection *conn, string data)
{
    data = "reply:" + data;
    int len = data.size();
    char tmpbuf[1024];
    memset(tmpbuf, len, 4);
    memcpy(tmpbuf + 4, data.c_str(), len);
    conn->send(tmpbuf, len + 4);
}
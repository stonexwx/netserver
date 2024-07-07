#include "EchoServer.h"

EchoServer::EchoServer(const string &ip, const string &port) : server_(ip, port)
{
    server_.setNewConnectionCallback(std::bind(&EchoServer::handleNewConnection, this, std::placeholders::_1));
    server_.setCloseConnectionCallback(std::bind(&EchoServer::handleCloseConnection, this, std::placeholders::_1));
    server_.setErrorConnectionCallback(std::bind(&EchoServer::handleErrorConnection, this, std::placeholders::_1));
    server_.setOnMessageCallback(std::bind(&EchoServer::handleOnMessage, this, std::placeholders::_1, std::placeholders::_2));
    server_.setSendCompleteCallback(std::bind(&EchoServer::handleSendComplete, this, std::placeholders::_1));
    server_.setEpollTimeoutCallback(std::bind(&EchoServer::handleEpollTimeout, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{
}

void EchoServer::start()
{
    server_.tcpServerStart();
}

void EchoServer::handleNewConnection(Socket *clientSocket)
{
    std::cout << "accept client(fd=" << clientSocket->getFd()
              << ",ip=" << clientSocket->getIp()
              << ",port=" << clientSocket->getPort()
              << ") ok." << std::endl;
}

void EchoServer::handleCloseConnection(Connection *conn)
{
    std::cout << "client(eventfd=" << conn->getFd() << ") disconnected." << std::endl;
}

void EchoServer::handleErrorConnection(Connection *conn)
{
    std::cout << "client(eventfd=" << conn->getFd() << ") disconnected." << std::endl;
}

void EchoServer::handleOnMessage(Connection *conn, string &data)
{
    data = "reply:" + data;

    conn->send(data.data(), data.size());
}

void EchoServer::handleSendComplete(Connection *conn)
{
    std::cout << "send complete." << std::endl;
}

void EchoServer::handleEpollTimeout(EventLoop *loop)
{
    std::cout << "epoll wait timeout." << std::endl;
}

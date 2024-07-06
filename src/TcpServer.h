#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <iostream>
#include <map>

#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"

class EventLoop;

class TcpServer
{
private:
    EventLoop loop_;
    Acceptor *acceptor_;
    std::map<int, Connection *> connMap_;

public:
    TcpServer(const string &ip, const string &port);
    ~TcpServer();

    void tcpServerStart();

    void newConnection(Socket *clientSocket);

    void closeConnection(Connection *conn);
    void errorConnection(Connection *conn);
};

#endif
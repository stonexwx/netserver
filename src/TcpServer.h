#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <iostream>
#include "EventLoop.h"

class EventLoop;

class TcpServer
{
private:
    EventLoop loop_;

public:
    TcpServer(const string &ip, const string &port);
    ~TcpServer();

    void tcpServerStart();
};

#endif
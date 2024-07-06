#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

class Connection
{
private:
    EventLoop *loop_;
    Socket *clientSocket_;
    Channel *clientChannel_;

public:
    Connection(EventLoop *loop, Socket *clientSocket);
    ~Connection();
};

#endif
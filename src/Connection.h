#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

class EventLoop;
class Socket;
class Channel;

class Connection
{
private:
    EventLoop *loop_;
    Socket *clientSocket_;
    Channel *clientChannel_;
    string clientIp_;
    uint16_t clientPort_;
    int fd_;

public:
    Connection(EventLoop *loop, Socket *clientSocket);
    ~Connection();

    string getClientIp() const;
    uint16_t getClientPort() const;
    int getFd() const;

    void closeCallback(); // 关闭回调函数
    void errorCallback(); // 错误回调函数
};

#endif
#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Connection.h"

class Socket;
class Channel;

class Acceptor
{
private:
    EventLoop *loop_;                                     // 指向EventLoop对象
    Socket *acceptSocket_;                                // 监听套接字
    Channel *acceptChannel_;                              // 监听套接字对应的Channel对象
    std::function<void(Socket *)> newConnectionCallback_; // 新连接回调函数
public:
    Acceptor(EventLoop *loop,
             const string &ip,
             const string &port);
    ~Acceptor();

    void newConnection();
    void setNewConnectionCallback(const std::function<void(Socket *)> &cb);
};

#endif
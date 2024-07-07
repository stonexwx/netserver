#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"

class EventLoop;
class Socket;
class Channel;

class Connection
{
private:
    EventLoop *loop_;
    Socket *clientSocket_;
    Channel *clientChannel_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;

    std::function<void(Connection *)> closeCallback_;
    std::function<void(Connection *)> errorCallback_;
    std::function<void(Connection *, string &)> onMessageCallback_;
    std::function<void(Connection *)> writeCompleteCallback_;

public:
    Connection(EventLoop *loop, Socket *clientSocket);
    ~Connection();

    string getClientIp() const;
    uint16_t getClientPort() const;
    int getFd() const;

    void send(const char *data, size_t size);

    void closeCallback();     // 关闭回调函数
    void errorCallback();     // 错误回调函数
    void onMessageCallback(); // 读回调函数
    void writeCallback();     // 写回调函数

    void setCloseCallback(const std::function<void(Connection *)> &cb);
    void setErrorCallback(const std::function<void(Connection *)> &cb);
    void setOnMessageCallback(const std::function<void(Connection *, string &)> &cb);
    void setWriteCompleteCallback(const std::function<void(Connection *)> &cb);
};

#endif
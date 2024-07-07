#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <memory>
#include <atomic>
#include <sys/syscall.h>

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"

class EventLoop;
class Socket;
class Channel;
class Connection;

using spConnection = std::shared_ptr<Connection>;

class Connection : public std::enable_shared_from_this<Connection>
{
private:
    EventLoop *loop_;
    std::unique_ptr<Socket> clientSocket_;
    std::unique_ptr<Channel> clientChannel_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
    std::atomic_bool disconnected_;

    std::function<void(spConnection)> closeCallback_;
    std::function<void(spConnection)> errorCallback_;
    std::function<void(spConnection, string &)> onMessageCallback_;
    std::function<void(spConnection)> writeCompleteCallback_;

public:
    Connection(EventLoop *loop, std::unique_ptr<Socket> clientSocket);
    ~Connection();

    string getClientIp() const;
    uint16_t getClientPort() const;
    int getFd() const;

    void send(const char *data, size_t size);
    void sendin(const string data, size_t size);

    void closeCallback();     // 关闭回调函数
    void errorCallback();     // 错误回调函数
    void onMessageCallback(); // 读回调函数
    void writeCallback();     // 写回调函数

    void setCloseCallback(const std::function<void(spConnection)> &cb);
    void setErrorCallback(const std::function<void(spConnection)> &cb);
    void setOnMessageCallback(const std::function<void(spConnection, string &)> &cb);
    void setWriteCompleteCallback(const std::function<void(spConnection)> &cb);
};

#endif
#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <iostream>
#include <map>
#include <memory>

#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"

class EventLoop;

class TcpServer
{
private:
    std::unique_ptr<EventLoop> mainLoop_; // 主EventLoop
    std::vector<std::unique_ptr<EventLoop>> loops_;
    Acceptor acceptor_;
    std::map<int, spConnection> connMap_;
    int threadNum_;
    ThreadPool threadPool_;

    std::function<void(spConnection)> newConnectionCallback_;
    std::function<void(spConnection)> closeConnectionCallback_;
    std::function<void(spConnection)> errorConnectionCallback_;
    std::function<void(spConnection, string &)> onMessageCallback_;
    std::function<void(spConnection)> sendCompleteCallback_;
    std::function<void(EventLoop *)> epollTimeoutCallback_;

public:
    TcpServer(const string &ip, const string &port, int treadNum = 3);
    ~TcpServer();

    void tcpServerStart();

    void newConnection(std::unique_ptr<Socket> clientSocket);

    void closeConnection(spConnection conn);
    void errorConnection(spConnection conn);
    void onMessage(spConnection conn, string &data);

    void sendComplete(spConnection conn);
    void epollTimeout(EventLoop *loop);

    void setNewConnectionCallback(const std::function<void(spConnection)> &cb);
    void setCloseConnectionCallback(const std::function<void(spConnection)> &cb);
    void setErrorConnectionCallback(const std::function<void(spConnection)> &cb);
    void setOnMessageCallback(const std::function<void(spConnection, string &)> &cb);
    void setSendCompleteCallback(const std::function<void(spConnection)> &cb);
    void setEpollTimeoutCallback(const std::function<void(EventLoop *)> &cb);
};

#endif
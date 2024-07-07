#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <iostream>
#include <map>

#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"

class EventLoop;

class TcpServer
{
private:
    EventLoop *mainLoop_; // 主EventLoop
    std::vector<EventLoop *> loops_;
    Acceptor *acceptor_;
    std::map<int, Connection *> connMap_;
    ThreadPool *threadPool_;
    int threadNum_;

    std::function<void(Socket *)> newConnectionCallback_;
    std::function<void(Connection *)> closeConnectionCallback_;
    std::function<void(Connection *)> errorConnectionCallback_;
    std::function<void(Connection *, string &)> onMessageCallback_;
    std::function<void(Connection *)> sendCompleteCallback_;
    std::function<void(EventLoop *)> epollTimeoutCallback_;

public:
    TcpServer(const string &ip, const string &port, int treadNum = 3);
    ~TcpServer();

    void tcpServerStart();

    void newConnection(Socket *clientSocket);

    void closeConnection(Connection *conn);
    void errorConnection(Connection *conn);
    void onMessage(Connection *conn, string &data);

    void sendComplete(Connection *conn);
    void epollTimeout(EventLoop *loop);

    void setNewConnectionCallback(const std::function<void(Socket *)> &cb);
    void setCloseConnectionCallback(const std::function<void(Connection *)> &cb);
    void setErrorConnectionCallback(const std::function<void(Connection *)> &cb);
    void setOnMessageCallback(const std::function<void(Connection *, string &)> &cb);
    void setSendCompleteCallback(const std::function<void(Connection *)> &cb);
    void setEpollTimeoutCallback(const std::function<void(EventLoop *)> &cb);
};

#endif
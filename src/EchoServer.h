#ifndef _ECHO_SERVER_H
#define _ECHO_SERVER_H

#include "TcpServer.h"
#include "ThreadPool.h"

class EchoServer
{
private:
    TcpServer server_;
    ThreadPool threadPool_;

public:
    EchoServer(const string &ip, const string &port,
               int subthreadNum = 3, int workThreadNum = 3);
    ~EchoServer();

    void start();
    void Stop();

    void handleNewConnection(spConnection conn);

    void handleCloseConnection(spConnection conn);
    void handleErrorConnection(spConnection conn);
    void handleOnMessage(spConnection conn, string &data);

    void handleSendComplete(spConnection conn);
    void handleEpollTimeout(EventLoop *loop);

    void onMessageInThreadPool(spConnection conn, string &data);
};

#endif
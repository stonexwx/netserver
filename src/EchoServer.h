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

    void handleNewConnection(Socket *clientSocket);

    void handleCloseConnection(Connection *conn);
    void handleErrorConnection(Connection *conn);
    void handleOnMessage(Connection *conn, string &data);

    void handleSendComplete(Connection *conn);
    void handleEpollTimeout(EventLoop *loop);

    void onMessageInThreadPool(Connection *conn, string &data);
};

#endif
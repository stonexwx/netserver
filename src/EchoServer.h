#ifndef _ECHO_SERVER_H
#define _ECHO_SERVER_H

#include "TcpServer.h"

class EchoServer
{
private:
    TcpServer server_;

public:
    EchoServer(const string &ip, const string &port);
    ~EchoServer();

    void start();

    void handleNewConnection(Socket *clientSocket);

    void handleCloseConnection(Connection *conn);
    void handleErrorConnection(Connection *conn);
    void handleOnMessage(Connection *conn, string& data);

    void handleSendComplete(Connection *conn);
    void handleEpollTimeout(EventLoop *loop);
};

#endif
#include "TcpServer.h"

TcpServer::TcpServer(const string &ip, const string &port, int treadNum)
    : threadNum_(treadNum), mainLoop_(new EventLoop(true)), acceptor_(mainLoop_.get(), ip, port), threadPool_(threadNum_, "tcp server")
{
    mainLoop_->setTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));

    acceptor_.setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));

    // 创建从事件循环

    for (int i = 0; i < threadNum_; i++)
    {
        loops_.emplace_back(new EventLoop(false));
        loops_[i]->setTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));
        threadPool_.addtask(std::bind(&EventLoop::run, loops_[i].get()));
    }
}

TcpServer::~TcpServer()
{
}

void TcpServer::tcpServerStart()
{
    mainLoop_->run();
}

void TcpServer::newConnection(std::unique_ptr<Socket> clientSocket)
{
    spConnection conn_(new Connection(loops_[clientSocket->getFd() % threadNum_].get(), std::move(clientSocket)));

    conn_->setCloseCallback(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
    conn_->setErrorCallback(std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
    conn_->setOnMessageCallback(std::bind(&TcpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    conn_->setWriteCompleteCallback(std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));
    connMap_[conn_->getFd()] = conn_;
    loops_[conn_->getFd() % threadNum_]->addConnection(conn_);

    if (newConnectionCallback_)
    {
        newConnectionCallback_(conn_);
    }
}

void TcpServer::closeConnection(spConnection conn)
{
    if (closeConnectionCallback_)
    {
        closeConnectionCallback_(conn);
    }
    auto iter = connMap_.find(conn->getFd());
    if (iter != connMap_.end())
    {
        connMap_.erase(iter);
    }
}

void TcpServer::errorConnection(spConnection conn)
{
    auto iter = connMap_.find(conn->getFd());
    if (iter != connMap_.end())
    {
        connMap_.erase(iter);
    }

    if (errorConnectionCallback_)
    {
        errorConnectionCallback_(conn);
    }
}

void TcpServer::onMessage(spConnection conn, string &data)
{
    if (onMessageCallback_)
    {
        onMessageCallback_(conn, data);
    }
}

void TcpServer::sendComplete(spConnection conn)
{
    if (sendCompleteCallback_)
    {
        sendCompleteCallback_(conn);
    }
}

void TcpServer::epollTimeout(EventLoop *loop)
{
    if (epollTimeoutCallback_)
    {
        epollTimeoutCallback_(loop);
    }
}

void TcpServer::setNewConnectionCallback(const std::function<void(spConnection)> &cb)
{
    newConnectionCallback_ = cb;
}

void TcpServer::setCloseConnectionCallback(const std::function<void(spConnection)> &cb)
{
    closeConnectionCallback_ = cb;
}

void TcpServer::setErrorConnectionCallback(const std::function<void(spConnection)> &cb)
{
    errorConnectionCallback_ = cb;
}

void TcpServer::setOnMessageCallback(const std::function<void(spConnection, string &)> &cb)
{
    onMessageCallback_ = cb;
}

void TcpServer::setSendCompleteCallback(const std::function<void(spConnection)> &cb)
{
    sendCompleteCallback_ = cb;
}

void TcpServer::setEpollTimeoutCallback(const std::function<void(EventLoop *)> &cb)
{
    epollTimeoutCallback_ = cb;
}

void TcpServer::removeconn(int fd)
{
    connMap_.erase(fd);
}

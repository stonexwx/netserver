#include "TcpServer.h"

TcpServer::TcpServer(const string &ip, const string &port, int treadNum) : threadNum_(treadNum)
{
    mainLoop_ = new EventLoop();
    mainLoop_->setTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));

    acceptor_ = new Acceptor(mainLoop_, ip, port);
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));

    threadPool_ = new ThreadPool(threadNum_, "tcp server"); // 创建线程池

    // 创建从事件循环

    for (int i = 0; i < threadNum_; i++)
    {
        loops_.push_back(new EventLoop);
        loops_[i]->setTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));
        threadPool_->addtask(std::bind(&EventLoop::run, loops_[i]));
    }
}

TcpServer::~TcpServer()
{
    delete acceptor_;
    delete mainLoop_;
    for (auto iter = connMap_.begin(); iter != connMap_.end(); ++iter)
    {
        delete iter->second;
    }
    for (auto loop : loops_)
    {
        delete loop;
    }
    delete threadPool_;
}

void TcpServer::tcpServerStart()
{
    mainLoop_->run();
}

void TcpServer::newConnection(Socket *clientSocket)
{
    // Connection *conn_ = new Connection(mainLoop_, clientSocket);
    Connection *conn_ = new Connection(loops_[connMap_.size() % threadNum_], clientSocket);

    // printf("accept client(fd=%d,ip=%s,port=%d) ok.\n",
    //        conn_->getFd(),
    //        conn_->getClientIp().c_str(),
    //        conn_->getClientPort());

    conn_->setCloseCallback(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
    conn_->setErrorCallback(std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
    conn_->setOnMessageCallback(std::bind(&TcpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    conn_->setWriteCompleteCallback(std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));
    connMap_[conn_->getFd()] = conn_;

    if (newConnectionCallback_)
    {
        newConnectionCallback_(clientSocket);
    }
}

void TcpServer::closeConnection(Connection *conn)
{
    if (closeConnectionCallback_)
    {
        closeConnectionCallback_(conn);
    }
    auto iter = connMap_.find(conn->getFd());
    if (iter != connMap_.end())
    {
        delete iter->second;
        connMap_.erase(iter);
    }
}

void TcpServer::errorConnection(Connection *conn)
{
    auto iter = connMap_.find(conn->getFd());
    if (iter != connMap_.end())
    {
        delete iter->second;
        connMap_.erase(iter);
    }

    if (errorConnectionCallback_)
    {
        errorConnectionCallback_(conn);
    }
}

void TcpServer::onMessage(Connection *conn, string &data)
{
    if (onMessageCallback_)
    {
        onMessageCallback_(conn, data);
    }
}

void TcpServer::sendComplete(Connection *conn)
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

void TcpServer::setNewConnectionCallback(const std::function<void(Socket *)> &cb)
{
    newConnectionCallback_ = cb;
}

void TcpServer::setCloseConnectionCallback(const std::function<void(Connection *)> &cb)
{
    closeConnectionCallback_ = cb;
}

void TcpServer::setErrorConnectionCallback(const std::function<void(Connection *)> &cb)
{
    errorConnectionCallback_ = cb;
}

void TcpServer::setOnMessageCallback(const std::function<void(Connection *, string &)> &cb)
{
    onMessageCallback_ = cb;
}

void TcpServer::setSendCompleteCallback(const std::function<void(Connection *)> &cb)
{
    sendCompleteCallback_ = cb;
}

void TcpServer::setEpollTimeoutCallback(const std::function<void(EventLoop *)> &cb)
{
    epollTimeoutCallback_ = cb;
}

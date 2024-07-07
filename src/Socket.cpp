#include "Socket.h"

int createNonblockingOrDie()
{
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd < 0)
    {
        printf("%s : %s : %d,socket() failed", __FILE__, __func__, __LINE__);
        exit(1);
    }
    return sockfd;
}

Socket::Socket(int fd) : sockfd_(fd)
{
    printf("Socket::Socket(int fd) : sockfd_(fd) : fd=%d\n", fd);
}

Socket::~Socket()
{
    close(sockfd_);
}

int Socket::getFd() const
{
    return sockfd_;
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::setNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::bindAddress(const InetAddress &addr)
{
    if (bind(sockfd_, addr.getAddr(), sizeof(sockaddr)) < 0)
    {
        printf("%s : %s : %d,bind() failed", __FILE__, __func__, __LINE__);
        close(sockfd_);
        exit(1);
    }
    setIpPort(addr.getIp(), addr.getPort());
}

void Socket::listen(int nn)
{
    if (::listen(sockfd_, nn) != 0) // 在高并发的网络服务器中，第二个参数要大一些。
    {
        printf("%s : %s : %d,listen() failed", __FILE__, __func__, __LINE__);
        close(sockfd_);
        exit(1);
    }
}

int Socket::accept(InetAddress &clientaddr)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int connfd = ::accept4(sockfd_, (sockaddr *)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0)
    {
        printf("%s : %s : %d,accept() failed:%s\n", __FILE__, __func__, __LINE__, strerror(errno));
        close(sockfd_);
        exit(1);
    }

    clientaddr.setAddr(addr);
    return connfd;
}

void Socket::shutdownWrite()
{
    if (shutdown(sockfd_, SHUT_WR) < 0)
    {
        printf("%s : %s : %d,shutdown() failed", __FILE__, __func__, __LINE__);
        close(sockfd_);
        exit(1);
    }
}

void Socket::setNonBlockAndCloseOnExec()
{
    int flags = fcntl(sockfd_, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(sockfd_, F_SETFL, flags);

    flags = fcntl(sockfd_, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    fcntl(sockfd_, F_SETFD, flags);
}

string Socket::getIp() const
{
    return ip_;
}

uint16_t Socket::getPort() const
{
    return port_;
}

void Socket::setIpPort(const string &ip, uint16_t port)
{
    ip_ = ip;
    port_ = port;
}
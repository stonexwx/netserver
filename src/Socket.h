#ifndef _SOCKET_H_
#define _SOCKET_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "InetAddress.h"

int createNonblockingOrDie(); // 创建非阻塞套接字

class Socket
{

private:
    const int sockfd_;

public:
    Socket(int fd);
    ~Socket();
    int getFd() const;
    void setReuseAddr(bool on); // 设置SO_REUSEADDR，允许重用本地地址
    void setReusePort(bool on); // 设置SO_REUSEPORT，允许多个套接字绑定到同一端口，对于某些使用场景有用
    void setNoDelay(bool on);   // 设置TCP_NODELAY，禁用Nagle算法，减少延迟
    void setKeepAlive(bool on); // 设置SO_KEEPALIVE，启用保活机制，可以检测到对端是否崩溃

    void bindAddress(const InetAddress &addr); // 绑定地址
    void listen(int nn = 128);                 // 监听
    int accept(InetAddress &clientaddr);       // 接受连接
    void shutdownWrite();                      // 关闭写端
    void setNonBlockAndCloseOnExec();          // 设置非阻塞和关闭exec
};

#endif

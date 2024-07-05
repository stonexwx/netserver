#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <string.h>

using namespace std;

int listenfd;

void socketInit(const unsigned int port)
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        cerr << "Error on binding" << endl;
        exit(1);
    }
    int opt = 1;
    // 设置SO_REUSEADDR，允许重用本地地址
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt)); // 必须的。

    // 设置TCP_NODELAY，禁用Nagle算法，减少延迟
    setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt)); // 必须的。

    // 设置SO_REUSEPORT，允许多个套接字绑定到同一端口，对于某些使用场景有用
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt)); // 有用，但是，在Reactor中意义不大。

    // 设置SO_KEEPALIVE，启用保活机制，可以检测到对端是否崩溃
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt)); // 可能有用，但是，建议自己做心跳。

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
}

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}

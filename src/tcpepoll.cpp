#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include "InetAddress.h"

using namespace std;

int socketInit()
{
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
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

    return listenfd;
}

int main(int argc, char const *argv[])
{
    // 初始化socket
    int listenfd = socketInit();
    if (argv[1] == NULL || argv[2] == NULL)
    {
        cerr << "Usage: " << argv[0] << " <ip> <port>" << endl;
        exit(1);
    }

    InetAddress servaddr(argv[1], atoi(argv[2]));

    if (bind(listenfd, servaddr.getAddr(), sizeof(sockaddr)) < 0)
    {
        perror("bind() failed");
        close(listenfd);
        return -1;
    }

    if (listen(listenfd, 128) != 0) // 在高并发的网络服务器中，第二个参数要大一些。
    {
        perror("listen() failed");
        close(listenfd);
        return -1;
    }

    int epollfd = epoll_create(1); // 创建epoll句柄（红黑树）。

    // 为服务端的listenfd准备读事件。
    epoll_event ev;        // 声明事件的数据结构。
    ev.data.fd = listenfd; // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
    ev.events = EPOLLIN;   // 让epoll监视listenfd的读事件，采用水平触发。

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev); // 把需要监视的listenfd和它的事件加入epollfd中。

    epoll_event evs[10]; // 存放epoll_wait()返回事件的数组。

    while (true) // 事件循环。
    {
        int infds = epoll_wait(epollfd, evs, 10, -1); // 等待监视的fd有事件发生。

        // 返回失败。
        if (infds < 0)
        {
            perror("epoll_wait() failed");
            break;
        }

        // 超时。
        if (infds == 0)
        {
            printf("epoll_wait() timeout.\n");
            continue;
        }

        // 如果infds>0，表示有事件发生的fd的数量。
        for (int ii = 0; ii < infds; ii++) // 遍历epoll返回的数组evs。
        {

            if (evs[ii].events & EPOLLRDHUP) // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
            {
                printf("client(eventfd=%d) disconnected.\n", evs[ii].data.fd);
                close(evs[ii].data.fd); // 关闭客户端的fd。
            } //  普通数据  带外数据
            else if (evs[ii].events & (EPOLLIN | EPOLLPRI)) // 接收缓冲区中有数据可以读。
            {
                if (evs[ii].data.fd == listenfd) // 如果是listenfd有事件，表示有新的客户端连上来。
                {

                    sockaddr_in clientSocket;
                    socklen_t len = sizeof(clientSocket);
                    int clientfd = accept4(listenfd, (sockaddr *)&clientSocket, &len, SOCK_NONBLOCK); // 客户端连接的fd必须设置为非阻塞的。
                    InetAddress clientaddr(clientSocket);
                    printf("accept client(fd=%d,ip=%s,port=%d) ok.\n", clientfd, clientaddr.getIp(), clientaddr.getPort());

                    // 为新客户端连接准备读事件，并添加到epoll中。
                    ev.data.fd = clientfd;
                    ev.events = EPOLLIN | EPOLLET; // 边缘触发。
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
                }
                else // 如果是客户端连接的fd有事件。
                {
                    char buffer[1024];
                    while (true) // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
                    {
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(evs[ii].data.fd, buffer, sizeof(buffer)); // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
                        if (nread > 0)                                                 // 成功的读取到了数据。
                        {
                            // 把接收到的报文内容原封不动的发回去。
                            printf("recv(eventfd=%d):%s\n", evs[ii].data.fd, buffer);
                            send(evs[ii].data.fd, buffer, strlen(buffer), 0);
                        }
                        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
                        {
                            continue;
                        }
                        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
                        {
                            break;
                        }
                        else if (nread == 0) // 客户端连接已断开。
                        {
                            printf("client(eventfd=%d) disconnected.\n", evs[ii].data.fd);
                            close(evs[ii].data.fd); // 关闭客户端的fd。
                            break;
                        }
                    }
                }
            }
            else if (evs[ii].events & EPOLLOUT) // 有数据需要写，暂时没有代码，以后再说。
            {
            }
            else // 其它事件，都视为错误。
            {
                printf("client(eventfd=%d) error.\n", evs[ii].data.fd);
                close(evs[ii].data.fd); // 关闭客户端的fd。
            }
            ////////////////////////////////////////////////////////////////////////
        }
    }

    return 0;
}

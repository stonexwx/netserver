#include <iostream>

#include "Socket.h"
#include "Epoll.h"

using namespace std;

int main(int argc, char const *argv[])
{
    // 初始化socket
    Socket socket(createNonblockingOrDie());
    if (argv[1] == NULL || argv[2] == NULL)
    {
        cerr << "Usage: " << argv[0] << " <ip> <port>" << endl;
        exit(1);
    }

    InetAddress servaddr(argv[1], atoi(argv[2]));
    socket.setKeepAlive(true);
    socket.setReuseAddr(true);
    socket.setReusePort(true);
    socket.setNoDelay(true);

    socket.bindAddress(servaddr);
    socket.listen();

    Epoll epoll;
    Channel *serveChannel = new Channel(&epoll, socket.getFd());
    serveChannel->enableReading();
    while (true) // 事件循环。
    {
        vector<Channel *> channels = epoll.loop(-1); // epoll_wait()，阻塞等待事件发生。
        // 如果infds>0，表示有事件发生的fd的数量。
        for (auto &channel : channels) // 遍历epoll返回的数组evs。
        {

            if (channel->getRevents() & EPOLLRDHUP) // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
            {
                printf("client(eventfd=%d) disconnected.\n", channel->getFd());
                close(channel->getFd()); // 关闭客户端的fd。
            } //  普通数据  带外数据
            else if (channel->getRevents() & (EPOLLIN | EPOLLPRI)) // 接收缓冲区中有数据可以读。
            {
                if (channel == serveChannel) // 如果是listenfd有事件，表示有新的客户端连上来。
                {
                    InetAddress clientaddr;
                    Socket *clientfd = new Socket(socket.accept(clientaddr)); // 接受新的客户端连接。

                    printf("accept client(fd=%d,ip=%s,port=%d) ok.\n", clientfd->getFd(), clientaddr.getIp(), clientaddr.getPort());

                    // 为新客户端连接准备读事件，并添加到epoll中。
                    Channel *clientChannel = new Channel(&epoll, clientfd->getFd());
                    clientChannel->enableReading();
                    clientChannel->useET(); // 使用ET模式。
                }
                else // 如果是客户端连接的fd有事件。
                {
                    char buffer[1024];
                    while (true) // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
                    {
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(channel->getFd(), buffer, sizeof(buffer)); // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
                        if (nread > 0)                                                  // 成功的读取到了数据。
                        {
                            // 把接收到的报文内容原封不动的发回去。
                            printf("recv(eventfd=%d):%s\n", channel->getFd(), buffer);
                            send(channel->getFd(), buffer, strlen(buffer), 0);
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
                            printf("client(eventfd=%d) disconnected.\n", channel->getFd());
                            close(channel->getFd()); // 关闭客户端的fd。
                            break;
                        }
                    }
                }
            }
            else if (channel->getRevents() & EPOLLOUT) // 有数据需要写，暂时没有代码，以后再说。
            {
            }
            else // 其它事件，都视为错误。
            {
                printf("client(eventfd=%d) error.\n", channel->getFd());
                close(channel->getFd()); // 关闭客户端的fd。
            }
            ////////////////////////////////////////////////////////////////////////
        }
    }

    return 0;
}

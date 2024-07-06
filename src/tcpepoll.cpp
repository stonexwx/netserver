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
    Channel *serveChannel = new Channel(&epoll, socket.getFd(), true);
    serveChannel->enableReading();
    while (true) // 事件循环。
    {
        vector<Channel *> channels = epoll.loop(-1); // epoll_wait()，阻塞等待事件发生。
        // 如果infds>0，表示有事件发生的fd的数量。
        for (auto &channel : channels) // 遍历epoll返回的数组evs。
        {
            channel->handleEvent(&socket); // 处理事件。
        }
    }

    return 0;
}

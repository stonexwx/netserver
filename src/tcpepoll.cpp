#include <iostream>

#include "Socket.h"
#include "Epoll.h"
#include "EventLoop.h"

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

    EventLoop loop;
    Channel *serveChannel = new Channel(&loop, socket.getFd());
    serveChannel->setReadCallback([&]()
                                  { serveChannel->newConnection(&socket); });
    serveChannel->enableReading();
    loop.run();

    return 0;
}

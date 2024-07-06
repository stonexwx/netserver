#include <iostream>

#include "TcpServer.h"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argv[1] == NULL || argv[2] == NULL)
    {
        cerr << "Usage: " << argv[0] << " <ip> <port>" << endl;
        exit(1);
    }

    TcpServer *server = new TcpServer(argv[1], argv[2]);

    server->tcpServerStart();

    return 0;
}

#include <iostream>

#include "EchoServer.h"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argv[1] == NULL || argv[2] == NULL)
    {
        cerr << "Usage: " << argv[0] << " <ip> <port>" << endl;
        exit(1);
    }

    EchoServer *server = new EchoServer(argv[1], argv[2]);

    server->start();

    return 0;
}

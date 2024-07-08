#include <iostream>
#include <signal.h>

#include "EchoServer.h"

using namespace std;

EchoServer *server;

void Stop(int signo)
{
    cout << "server stop" << endl;
    server->Stop();
    delete server;
    exit(0);
}

int main(int argc, char const *argv[])
{
    if (argv[1] == NULL || argv[2] == NULL)
    {
        cerr << "Usage: " << argv[0] << " <ip> <port>" << endl;
        exit(1);
    }

    signal(SIGINT, Stop);
    signal(SIGTERM, Stop);

    server = new EchoServer(argv[1], argv[2], 3, 3);

    server->start();

    return 0;
}

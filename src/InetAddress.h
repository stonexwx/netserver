#ifndef _INETADDRESS_H_
#define _INETADDRESS_H_

#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include <strings.h>

using namespace std;

class InetAddress
{
private:
    struct sockaddr_in addr_;

public:
    InetAddress(const string &ip, uint16_t port = 10000);
    InetAddress(const struct sockaddr_in addr);
    ~InetAddress();

    const char *getIp() const;              // 获取IP地址
    uint16_t getPort() const;               // 获取端口号
    const struct sockaddr *getAddr() const; // 获取地址信息
};

#endif
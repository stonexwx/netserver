#include "InetAddress.h"

InetAddress::InetAddress(const string &ip, uint16_t port)
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddress::InetAddress(const struct sockaddr_in addr)
{
    addr_ = addr;
}

InetAddress::~InetAddress()
{
}

const char *InetAddress::getIp() const
{
    return inet_ntoa(addr_.sin_addr);
}

uint16_t InetAddress::getPort() const
{
    return ntohs(addr_.sin_port);
}

const struct sockaddr *InetAddress::getAddr() const
{
    return (sockaddr *)&addr_;
}

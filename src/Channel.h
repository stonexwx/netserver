#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"

class Epoll;

class Channel
{
private:
    int fd_ = -1;          // 文件描述符
    Epoll *ep_ = nullptr;  // 指向epoll对象
    bool inepoll_ = false; // 是否在epoll中
    uint32_t events_ = 0;  // 需要监听的事件
    uint32_t revents_ = 0; // 实际发生的事件
    bool isListen_ = false;

public:
    Channel(Epoll *ep, int fd, bool isListen = false);
    ~Channel();

    int getFd() const;
    void useET();                      // 设置EPOLLET
    void enableReading();              // 开启读事件监听
    void setInEpoll();                 // 设置是否在epoll中
    void setrevents(uint32_t revents); // 设置发生的事件
    bool inpoll() const;               // 是否在epoll中
    uint32_t getEvents() const;        // 获取需要监听的事件
    uint32_t getRevents() const;       // 获取发生的事件

    void handleEvent(Socket *serveSocket); // 处理事件
};

#endif
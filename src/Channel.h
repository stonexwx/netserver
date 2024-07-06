#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>

#include "InetAddress.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Connection.h"

class EventLoop;
class Channel
{
private:
    int fd_ = -1;                    // 文件描述符
    EventLoop *loop_ = nullptr;      // 指向EventLoop对象
    bool inepoll_ = false;           // 是否在epoll中
    uint32_t events_ = 0;            // 需要监听的事件
    uint32_t revents_ = 0;           // 实际发生的事件
    function<void()> readCallback_;  // 读回调函数
    function<void()> closeCallback_; // 关闭回调函数
    function<void()> errorCallback_; // 错误回调函数

public:
    Channel(EventLoop *loop, int fd);
    ~Channel();

    int getFd() const;
    void useET();                      // 设置EPOLLET
    void enableReading();              // 开启读事件监听
    void setInEpoll();                 // 设置是否在epoll中
    void setrevents(uint32_t revents); // 设置发生的事件
    bool inpoll() const;               // 是否在epoll中
    uint32_t getEvents() const;        // 获取需要监听的事件
    uint32_t getRevents() const;       // 获取发生的事件

    void handleEvent(); // 处理事件

    void handleRead(); // 读事件

    void setReadCallback(const function<void()> &cb); // 设置读回调函数

    void setCloseCallback(const function<void()> &cb); // 设置关闭回调函数
    void setErrorCallback(const function<void()> &cb); // 设置错误回调函数
};

#endif
#ifndef _BUFFER_H
#define _BUFFER_H

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <string.h>

using namespace std;

class Buffer
{
private:
    string buffer_;
    const uint16_t sep_; // 报文分割形式：0-无分割，1-4字节报文头，2-换行符
public:
    Buffer(uint16_t sep = 1);
    ~Buffer();

    void append(const char *data, size_t size);
    void appendWithSep(const char *data, size_t size);

    void erase(size_t pos, size_t len);

    size_t
    size();
    const char *data();
    void clear();

    bool pickMessage(string &message);
};

#endif
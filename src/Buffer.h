#ifndef _BUFFER_H
#define _BUFFER_H

#include <iostream>
#include <string>

using namespace std;

class Buffer
{
private:
    string buffer_;

public:
    Buffer(/* args */);
    ~Buffer();

    void append(const char *data, size_t size);
    void appendWithHead(const char *data, size_t size);

    void erase(size_t pos, size_t len);

    size_t
    size();
    const char *data();
    void clear();
};

#endif
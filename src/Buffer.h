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

    void append(const string &data, size_t size);
    size_t size();
    const char *data();
    void clear();
};

#endif
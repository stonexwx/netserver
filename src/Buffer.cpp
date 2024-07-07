#include "Buffer.h"

Buffer::Buffer(/* args */)
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const string &data, size_t size)
{
    buffer_.append(data, size);
}

size_t Buffer::size()
{
    return buffer_.size();
}

const char *Buffer::data()
{
    return buffer_.data();
}

void Buffer::clear()
{
    buffer_.clear();
}
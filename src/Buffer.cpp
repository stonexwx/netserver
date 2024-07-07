#include "Buffer.h"

Buffer::Buffer(/* args */)
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const char *data, size_t size)
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

void Buffer::erase(size_t pos, size_t len)
{
    buffer_.erase(pos, len);
}
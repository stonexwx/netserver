#include "Buffer.h"

Buffer::Buffer(uint16_t sep) : sep_(sep)
{
}

Buffer::~Buffer()
{
}

void Buffer::append(const char *data, size_t size)
{
    buffer_.append(data, size);
}

void Buffer::appendWithSep(const char *data, size_t size)
{
    if (sep_ == 0)
    {
        buffer_.append(data, size);
    }
    else if (sep_ == 1)
    {
        buffer_.append(reinterpret_cast<char *>(&size), 4);
        buffer_.append(data, size);
    }
    else if (sep_ == 2)
    {
        buffer_.append(data, size);
        buffer_.append("\r\n", 2);
    }
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

bool Buffer::pickMessage(string &message)
{
    if (buffer_.empty())
    {
        return false;
    }
    if (sep_ == 0)
    {
        message = buffer_;
        buffer_.clear();
        return true;
    }
    else if (sep_ == 1)
    {
        int len;
        memcpy(&len, buffer_.data(), 4);
        if (buffer_.size() < static_cast<size_t>(len) + 4)
        {
            return false;
        }
        message.assign(buffer_.data() + 4, len);
        buffer_.erase(0, len + 4);
        return true;
    }
    else if (sep_ == 2)
    {
        size_t pos = buffer_.find("\r\n");
        if (pos == string::npos)
        {
            return false;
        }
        message = buffer_.substr(0, pos);
        buffer_.erase(0, pos + 2);
        return true;
    }
    return false;
}
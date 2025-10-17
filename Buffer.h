#pragma once
#include <iostream>
#include <string>

class Buffer
{
private:
    std::string m_buffer;

public:
    Buffer();
    ~Buffer();

    void append(const char *data, size_t size); // 把数据追加到Buffer中
    void erase(size_t pos, size_t len);         // 删除Buffer中数据,从pos开始,长度为len
    size_t size();                              // 获取Buffer中数据的大小
    const char *data();                         // 获取Buffer中数据的起始地址
    void clear();                               // 清空Buffer
};
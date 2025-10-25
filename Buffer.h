#pragma once
#include <iostream>
#include <string>
#include <cstring>

class Buffer
{
private:
    std::string m_buffer;
    const u_int16_t m_sep; // 报文的分割符: 0-无分隔符; 1-四字节的报文头; 2-“\r\n\r\n”分隔符(http协议)

public:
    Buffer(u_int16_t sep = 1);
    ~Buffer();

    void append(const char *data, size_t size); // 把数据追加到Buffer中
    void appendWithSep(const char *data, size_t size); // 把数据追加到Buffer中,并附加报文头部
    void erase(size_t pos, size_t len);         // 删除Buffer中数据,从pos开始,长度为len
    size_t size();                              // 获取Buffer中数据的大小
    const char *data();                         // 获取Buffer中数据的起始地址
    void clear();                               // 清空Buffer
    bool pickMessage(std::string &message);     // 从m_buffer中拆分出一个报文,存放在message中,如果没有,则返回false
};
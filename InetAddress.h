#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>


class InetAddress {
private:
    struct sockaddr_in m_addr;  // 地址结构体
public:
    InetAddress();
    InetAddress(const std::string &ip, uint16_t port);  // 服务端监听fd
    InetAddress(sockaddr_in addr);                      // 客户端fd

    ~InetAddress();                                     // 析构函数

    const struct sockaddr *getSockAddr() const;         // 获取地址结构体
    const socklen_t getSockLen() const;                 // 获取地址结构体长度
    const char *getIp() const;                          // 获取ip
    uint16_t getPort() const;                           // 获取端口
    void setAddr(sockaddr_in addr);                     // 设置地址结构体
};
#include "InetAddress.h"

InetAddress::InetAddress() {}

// 服务端监听fd
InetAddress::InetAddress(const std::string &ip, uint16_t port) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_addr.sin_port = htons(port);
}

// 客户端fd
InetAddress::InetAddress(sockaddr_in addr):m_addr(addr) {}

// 析构函数
InetAddress::~InetAddress() {}

// 获取sockaddr
const struct sockaddr *InetAddress::getSockAddr() const {
    return (struct sockaddr *)&m_addr;
}

// 获取socklen
const socklen_t InetAddress::getSockLen() const {
    return sizeof(m_addr);

}

// 获取ip
const char *InetAddress::getIp() const {
    return inet_ntoa(m_addr.sin_addr);
}

// 获取port
uint16_t InetAddress::getPort() const {
    return ntohs(m_addr.sin_port);
}

// 设置地址结构体            
void InetAddress::setAddr(sockaddr_in addr) {
    m_addr = addr;
}
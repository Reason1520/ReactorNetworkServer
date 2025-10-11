#pragma once
#include "InetAddress.h"
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/tcp.h>

int createNonblockingSocket();

class Socket {
private:
    int m_fd;                       // Socket持有的fd
public:
    Socket(int fd);                 // 构造函数
    ~Socket();                      // 析构函数

    int getFd() const;              // 获取fd
    void setReuseaddr(bool on);     // 设置地址复用
    void setReuseport(bool on);     // 设置端口复用
    void setKeepalive(bool on);     // 设置心跳检测
    void setTcpNoDelay(bool on);    // 关闭Nagle算法

    void bindAddress(const InetAddress &server_addr);   // 绑定地址
    void listen(int size = 128);    // 监听
    int accept(InetAddress &client_addr);               // 接收连接
};
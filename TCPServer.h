#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

class TCPServer {
private:
    EventLoop loop; // 事件循环对象
public:
    TCPServer(const std::string &ip, const uint16_t port);  // 构造函数
    ~TCPServer();                                           // 析构函数

    void start();                                           // 启动服务
};
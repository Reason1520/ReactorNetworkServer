#pragma once
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class Connection
{
private:
    Socket *m_client_socket;   // 连接套接字,在构造函数中创建
    Channel *m_client_channel; // Connetion对应的Channel,在构造函数中创建
    EventLoop *m_loop;         // 运行loop的EventLoop,由外部传入
public:
    Connection(EventLoop *loop, Socket *client_socket); // 构造函数
    ~Connection();                                      // 析构函数
};
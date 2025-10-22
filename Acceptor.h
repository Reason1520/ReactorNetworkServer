#pragma once
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Connection.h"
#include <memory>

class Acceptor
{
private:
    Socket m_listen_socket;                                     // 监听套接字,在构造函数中创建
    Channel m_listen_channel;                                   // Acceptor对应的Channel,在构造函数中创建
    EventLoop * m_loop;                                         // 运行loop的EventLoop,由外部传入
    std::function<void(std::unique_ptr<Socket>)> m_new_connection_callback;    // 新连接回调函数,由外部传入
public:
    Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port);      // 构造函数
    ~Acceptor();                                                                // 析构函数
    void handleNewConnection();                                                 // 创建新的连接
    void setNewConnectionCallback(std::function<void(std::unique_ptr<Socket>)> callback);   // 设置新连接回调函数
};
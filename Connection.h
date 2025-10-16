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
    std::function<void(Connection *)> m_close_callback; // 关闭连接的回调函数,将回调TCPServer::m_close_callback;
    std::function<void(Connection *)> m_error_callback; // 错误处理回调函数,将回调TCPServer::m_error_callback;
public:
    Connection(EventLoop *loop, Socket *client_socket); // 构造函数
    ~Connection();                                      // 析构函数
    int getFd() const;                                  // 获取fd
    std::string getIp() const;                          // 获取ip
    uint16_t getPort() const;                           // 获取端口

    void close_callback();                              // 关闭连接的回调函数,供Channel调用
    void error_callback();                              // 错误处理回调函数,供Channel调用
    
    void setCloseCallback(std::function<void(Connection *)> callback);  // 设置关闭连接的回调函数
    void setErrorCallback(std::function<void(Connection *)> callback);  // 设置错误处理回调函数
};
#pragma once
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"

class Connection
{
private:
    Socket *m_client_socket;    // 连接套接字,在构造函数中创建
    Channel *m_client_channel;  // Connetion对应的Channel,在构造函数中创建
    EventLoop *m_loop;          // 运行loop的EventLoop,由外部传入
    Buffer m_input_buffer;      // 接收缓冲区
    Buffer m_output_buffer;     // 发送缓冲区

    std::function<void(Connection *)> m_close_callback;     // 关闭连接的回调函数,将回调TCPServer::m_closeConnection;
    std::function<void(Connection *)> m_error_callback;     // 错误处理回调函数,将回调TCPServer::m_errorConnection;
    std::function<void(Connection *, std::string&)> m_handle_message_callback;   // 处理对端发送过来的数据的回调函数,将回调TCPServer::m_handleMessage;
    std::function<void(Connection *)> m_send_complete_callback; // 发送数据完成回调函数,将回调TCPServer::m_sendComplete;
public:
    Connection(EventLoop *loop, Socket *client_socket); // 构造函数
    ~Connection();                                      // 析构函数
    int getFd() const;                                  // 获取fd
    std::string getIp() const;                          // 获取ip
    uint16_t getPort() const;                           // 获取端口

    void handleMessage();                               // 处理对端发送过来的数据
    void close_callback();                              // 关闭连接的回调函数,供Channel调用
    void error_callback();                              // 错误处理回调函数,供Channel调用   
    void write_callback();                              // 发送缓冲区数据可写时的回调函数,供Channel调用
    
    void setCloseCallback(std::function<void(Connection *)> callback);                      // 设置关闭连接的回调函数
    void setErrorCallback(std::function<void(Connection *)> callback);                      // 设置错误处理回调函数
    void setHandleMessageCallback(std::function<void(Connection *, std::string&)> callback); // 设置处理对端发送过来的数据的回调函数
    void setSendCompleteCallback(std::function<void(Connection *)> callback);               // 设置发送数据完成回调函数

    void send(const char *data, size_t size);   // 发送数据
};
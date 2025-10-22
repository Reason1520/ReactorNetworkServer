#pragma once
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include <memory>
#include <atomic>

class Connection;   // 前置声明
using spConnection = std::shared_ptr<Connection>;   // Connection智能指针

// 继承自std::enable_shared_from_this,使得Connection类可以创建智能指针
class Connection:public std::enable_shared_from_this<Connection>
{
private:
    std::unique_ptr<Socket> m_client_socket;    // 连接套接字
    std::unique_ptr<Channel> m_client_channel;  // Connetion对应的Channel,在构造函数中创建
    EventLoop *m_loop;              // 运行loop的EventLoop,由外部传入
    Buffer m_input_buffer;          // 接收缓冲区
    Buffer m_output_buffer;         // 发送缓冲区
    std::atomic_bool m_diconnect;   // 是否已经关闭连接

    std::function<void(spConnection)> m_close_callback;           // 关闭连接的回调函数,将回调TCPServer::m_closeConnection;
    std::function<void(spConnection)> m_error_callback;           // 错误处理回调函数,将回调TCPServer::m_errorConnection;
    std::function<void(spConnection, std::string&)> m_handle_message_callback;   // 处理对端发送过来的数据的回调函数,将回调TCPServer::m_handleMessage;
    std::function<void(spConnection)> m_send_complete_callback;   // 发送数据完成回调函数,将回调TCPServer::m_sendComplete;
public:
    Connection(EventLoop *loop, std::unique_ptr<Socket> client_socket); // 构造函数
    ~Connection();                                      // 析构函数
    int getFd() const;                                  // 获取fd
    std::string getIp() const;                          // 获取ip
    uint16_t getPort() const;                           // 获取端口

    void handleMessage();                               // 处理对端发送过来的数据
    void close_callback();                              // 关闭连接的回调函数,供Channel调用
    void error_callback();                              // 错误处理回调函数,供Channel调用   
    void write_callback();                              // 发送缓冲区数据可写时的回调函数,供Channel调用
    
    void setCloseCallback(std::function<void(spConnection)> callback);                        // 设置关闭连接的回调函数
    void setErrorCallback(std::function<void(spConnection)> callback);                        // 设置错误处理回调函数
    void setHandleMessageCallback(std::function<void(spConnection, std::string&)> callback);  // 设置处理对端发送过来的数据的回调函数
    void setSendCompleteCallback(std::function<void(spConnection)> callback);                 // 设置发送数据完成回调函数

    void send(const char *data, size_t size);           // 发送数据,不管是在线程中还是IO线程中,都调用这个函数
    void send_in_loop(const char *data, size_t size);   // 发送数据,如果是在IO线程中,则直接调用这个函数,如果是在工作线程中,则把这个函数放到IO线程中执行
};
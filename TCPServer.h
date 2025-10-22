#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <map>
#include <memory>

class TCPServer {
private:
    std::unique_ptr<EventLoop> m_main_loop;                 // 主事件循环对象
    std::vector<std::unique_ptr<EventLoop>> m_sub_loops;    // 从事件循环对象集合
    int m_thread_num;                                       // 线程池大小
    ThreadPool m_thread_pool;                               // 线程池对象
    Acceptor m_acceptor;                                    // 连接接收器对象
    std::map<int, spConnection> m_connections;              // 连接对象集合(fd,Connection)

    std::function<void(spConnection)> m_new_connection_callback;                // 回调EchoServer::HandleNewConnection
    std::function<void(spConnection)> m_close_connection_callback;              // 回调EchoServer::HandleCloseConnection
    std::function<void(spConnection)> m_error_connection_callback;              // 回调EchoServer::HandleErrorConnection
    std::function<void(spConnection, std::string &)> m_handle_message_callback; // 回调EchoServer::HandleMessage
    std::function<void(spConnection)> m_send_complete_callback;                 // 回调EchoServer::HandleSendComplete
    std::function<void(EventLoop *)> m_epoll_time_out_callback;                 // 回调EchoServer::HandleEpollTimeOut
public:
    TCPServer(const std::string &ip, const uint16_t port, int thread_num);  // 构造函数
    ~TCPServer();                                                               // 析构函数

    void start();                                                               // 启动服务

    void newConnection(std::unique_ptr<Socket> client_socket);         // 处理新客户端连接请求,在Acceptor类中回调
    void closeConnection(spConnection connection);                      // 关闭连接,在Connection类中回调
    void errorConnection(spConnection connection);                      // 处理连接异常,在Connection类中回调
    void handleMessage(spConnection connection, std::string &message);  // 处理客户端请求报文,在Connection类中回调
    void sendComplete(spConnection connection);                         // 发送完成,在Connection类中回调
    void epollTimeOut(EventLoop *loop);                                 // epoll_wait,在EventLoop类中回调

    void setNewConnectionCallback(std::function<void(spConnection)> callback);      // 设置回调EchoServer::HandleNewConnection
    void setCloseConnectionCallback(std::function<void(spConnection)> callback);    // 设置回调EchoServer::HandleCloseConnection
    void setErrorConnectionCallback(std::function<void(spConnection)> callback);    // 设置回调EchoServer::HandleErrorConnection
    void setHandleMessageCallback(std::function<void(spConnection, std::string &)> callback); // 设置回调EchoServer::HandleMessage
    void setSendCompleteCallback(std::function<void(spConnection)> callback);       // 设置回调EchoServer::HandleSendComplete
    void setEpollTimeOutCallback(std::function<void(EventLoop *)> callback);        // 设置回调EchoServer::HandleEpollTimeOut
};
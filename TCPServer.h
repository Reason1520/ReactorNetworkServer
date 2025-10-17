#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include <map>

class TCPServer {
private:
    EventLoop m_loop;                           // 事件循环对象
    Acceptor *m_acceptor;                       // 连接接收器对象
    std::map<int, Connection *> m_connections;  // 连接对象集合(fd,Connection)

    std::function<void(Connection *)> m_new_connection_callback;                // 回调EchoServer::HandleNewConnection
    std::function<void(Connection *)> m_close_connection_callback;              // 回调EchoServer::HandleCloseConnection
    std::function<void(Connection *)> m_error_connection_callback;              // 回调EchoServer::HandleErrorConnection
    std::function<void(Connection *, std::string)> m_handle_message_callback;   // 回调EchoServer::HandleMessage
    std::function<void(Connection *)> m_send_complete_callback;                 // 回调EchoServer::HandleSendComplete
    std::function<void(EventLoop *)> m_epoll_time_out_callback;                 // 回调EchoServer::HandleEpollTimeOut
public:
    TCPServer(const std::string &ip, const uint16_t port);  // 构造函数
    ~TCPServer();                                           // 析构函数

    void start();                                           // 启动服务

    void newConnection(Socket *client_socket);                          // 处理新客户端连接请求,在Acceptor类中回调
    void closeConnection(Connection *connection);                       // 关闭连接,在Connection类中回调
    void errorConnection(Connection *connection);                       // 处理连接异常,在Connection类中回调
    void handleMessage(Connection *connection, std::string message);    // 处理客户端请求报文,在Connection类中回调
    void sendComplete(Connection *connection);                          // 发送完成,在Connection类中回调
    void epollTimeOut(EventLoop *loop);                                 // epoll_wait,在EventLoop类中回调

    void setNewConnectionCallback(std::function<void(Connection *)> callback);      // 设置回调EchoServer::HandleNewConnection
    void setCloseConnectionCallback(std::function<void(Connection *)> callback);    // 设置回调EchoServer::HandleCloseConnection
    void setErrorConnectionCallback(std::function<void(Connection *)> callback);    // 设置回调EchoServer::HandleErrorConnection
    void setHandleMessageCallback(std::function<void(Connection *, std::string)> callback); // 设置回调EchoServer::HandleMessage
    void setSendCompleteCallback(std::function<void(Connection *)> callback);       // 设置回调EchoServer::HandleSendComplete
    void setEpollTimeOutCallback(std::function<void(EventLoop *)> callback);        // 设置回调EchoServer::HandleEpollTimeOut
};
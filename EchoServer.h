#pragma once
#include "TCPServer.h"
#include "EventLoop.h"
#include "Connection.h"

/*
    EchoServer类: 回显服务器
*/

class EchoServer
{
private:
    TCPServer m_tcp_server;
public:
    EchoServer(const std::string &ip, const uint16_t port ,int thread_num = 3);
    ~EchoServer();

    void Start();   // 启动服务

    void HandleNewConnection(Connection *connection);                   // 处理新客户端连接请求,在TCPServer类中回调
    void HandleCloseConnection(Connection *connection);                 // 关闭客户端的连接,在TCPServer类中回调
    void HandleErrorConnection(Connection *connection);                 // 客户端的连接错误,在TCPServer类中回调
    void HandleMessage(Connection *connection, std::string &message);    // 处理客户端得请求报文,在TCPServer类中回调
    void HandleSendComplete(Connection *connection);                    // 数据发送完成,在TCPServer类中回调
    void HandleEpollTimeOut(EventLoop *loop);                           // epoll_wait超时,在TCPServer类中回调
};


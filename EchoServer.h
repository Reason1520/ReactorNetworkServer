#pragma once
#include "TCPServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "TimeStamp.h"

/*
    EchoServer类: 回显服务器
*/

class EchoServer
{
private:
    TCPServer m_tcp_server;
    ThreadPool m_thread_pool;   // 工作线程池
public:
    EchoServer(const std::string &ip, const uint16_t port ,int sub_thread_num = 3, int work_thread_num = 5);
    ~EchoServer();

    void Start();   // 启动服务
    void Stop();    // 停止服务

    void HandleNewConnection(spConnection connection);                  // 处理新客户端连接请求,在TCPServer类中回调
    void HandleCloseConnection(spConnection connection);                // 关闭客户端的连接,在TCPServer类中回调
    void HandleErrorConnection(spConnection connection);                // 客户端的连接错误,在TCPServer类中回调
    void HandleMessage(spConnection connection, std::string &message);  // 处理客户端得请求报文,在TCPServer类中回调
    void HandleSendComplete(spConnection connection);                   // 数据发送完成,在TCPServer类中回调
    void HandleEpollTimeOut(EventLoop *loop);                           // epoll_wait超时,在TCPServer类中回调

    void HandleMessage_thread(spConnection connection, std::string &message); // 处理客户端得请求报文,用于添加给线程池
};


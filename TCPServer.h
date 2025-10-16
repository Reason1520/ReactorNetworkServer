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
public:
    TCPServer(const std::string &ip, const uint16_t port);  // 构造函数
    ~TCPServer();                                           // 析构函数

    void start();                                           // 启动服务
    void newConnection(Socket *client_socket);              // 处理新客户端连接请求,在Acceptor类中回调
    void closeConnection(Connection *connection);           // 关闭连接,在Connection类中回调
    void errorConnection(Connection *connection);           // 处理连接异常,在Connection类中回调
};
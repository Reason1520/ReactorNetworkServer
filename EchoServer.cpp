#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port) : m_tcp_server(ip, port) {
    m_tcp_server.setNewConnectionCallback(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    m_tcp_server.setCloseConnectionCallback(std::bind(&EchoServer::HandleCloseConnection, this, std::placeholders::_1));
    m_tcp_server.setErrorConnectionCallback(std::bind(&EchoServer::HandleErrorConnection, this, std::placeholders::_1));
    m_tcp_server.setHandleMessageCallback(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    m_tcp_server.setSendCompleteCallback(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    m_tcp_server.setEpollTimeOutCallback(std::bind(&EchoServer::HandleEpollTimeOut, this, std::placeholders::_1));
}

EchoServer::~EchoServer() {

}

// 启动服务
void EchoServer::Start() {
    m_tcp_server.start();
}

// 处理新客户端连接请求,在TCPServer类中回调
void EchoServer::HandleNewConnection(Connection *connection) {
    std::cout << "New Connection from " << connection->getIp() << ":" << connection->getPort() << std::endl;
}

// 关闭客户端的连接,在TCPServer类中回调
void EchoServer::HandleCloseConnection(Connection *connection) {
    std::cout << "Close Connection from " << connection->getIp() << ":" << connection->getPort() << std::endl;
}

// 客户端的连接错误,在TCPServer类中回调
void EchoServer::HandleErrorConnection(Connection *connection) {
    std::cout << "Error Connection from " << connection->getIp() << ":" << connection->getPort() << std::endl;
}

// 处理客户端得请求报文,在TCPServer类中回调
void EchoServer::HandleMessage(Connection *connection, std::string message) {
    message = "回复: " + message;         // 回显业务
    int len = message.size();             // 计算回应报文长度
    std::string tempbuf((char *)&len, 4); // 把报文头部填充到回应报文中
    tempbuf.append(message);              // 把报文内容填充到回应报文中

    connection->send(tempbuf.c_str(), tempbuf.size()); // 把临时缓冲区的数据发送给客户端
}

// 数据发送完成,在TCPServer类中回调
void EchoServer::HandleSendComplete(Connection *connection) {
    std::cout << "Send Complete to " << connection->getIp() << ":" << connection->getPort() << std::endl;
}

// epoll_wait超时,在TCPServer类中回调
void EchoServer::HandleEpollTimeOut(EventLoop *loop) {
    std::cout << "Epoll Time Out" << std::endl;
}
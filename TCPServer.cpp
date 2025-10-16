#include "TCPServer.h"

// 构造函数
TCPServer::TCPServer(const std::string &ip, const uint16_t port) {
    m_acceptor = new Acceptor(&m_loop, ip, port);                                                           // 创建连接接收器对象
    m_acceptor->setNewConnectionCallback(std::bind(&TCPServer::newConnection, this, std::placeholders::_1));// 设置新连接回调函数
}

// 析构函数
TCPServer::~TCPServer() {
    delete m_acceptor;
}

// 启动服务器
void TCPServer::start() {
    m_loop.run();
}

// 处理新客户端连接请求
void TCPServer::newConnection(Socket *client_socket) {
    Connection *connection = new Connection(&m_loop, client_socket); // 创建连接对象(未释放)
}
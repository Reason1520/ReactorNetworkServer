#include "TCPServer.h"

// 构造函数
TCPServer::TCPServer(const std::string &ip, const uint16_t port) {
    m_acceptor = new Acceptor(&m_loop, ip, port);                                                           // 创建连接接收器对象
    m_acceptor->setNewConnectionCallback(std::bind(&TCPServer::newConnection, this, std::placeholders::_1));// 设置新连接回调函数
}

// 析构函数
TCPServer::~TCPServer() {
    delete m_acceptor;
    for (auto &connection : m_connections) {
        delete connection.second;
    }
}

// 启动服务器
void TCPServer::start() {
    m_loop.run();
}

// 处理新客户端连接请求
void TCPServer::newConnection(Socket *client_socket) {
    Connection *connection = new Connection(&m_loop, client_socket);                                    // 创建连接对象
    connection->setCloseCallback(std::bind(&TCPServer::closeConnection, this, std::placeholders::_1));  // 设置连接关闭回调函数
    connection->setErrorCallback(std::bind(&TCPServer::errorConnection, this, std::placeholders::_1));  // 设置连接异常回调函数

    printf("新连接: fd %d, ip %s:%d\n", connection->getFd(), connection->getIp().c_str(), connection->getPort());

    m_connections.insert(std::make_pair(connection->getFd(), connection));
}

// 关闭连接,在Connection类中回调
void TCPServer::closeConnection(Connection *connection) {
    printf("关闭连接: fd %d, ip %s:%d\n", connection->getFd(), connection->getIp().c_str(), connection->getPort());
    m_connections.erase(connection->getFd());   // 删除连接对象
    delete connection;                          // 释放连接对象
}

// 处理连接异常, 在Connection类中回调
void TCPServer::errorConnection(Connection *connection) {
    printf("连接异常: fd %d, ip %s:%d\n", connection->getFd(), connection->getIp().c_str(), connection->getPort());
    m_connections.erase(connection->getFd());   // 删除连接对象
    delete connection;                          // 释放连接对象
}
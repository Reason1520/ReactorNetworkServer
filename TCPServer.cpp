#include "TCPServer.h"

// 构造函数
TCPServer::TCPServer(const std::string &ip, const uint16_t port, int thread_num) :m_thread_num(thread_num) {
    m_main_loop = new EventLoop();                                                                              // 创建主事件循环对象
    m_main_loop->setEpollTimeOutCallback(std::bind(&TCPServer::epollTimeOut, this, std::placeholders::_1));     // 设置epoll_wait超时回调函数

    m_acceptor = new Acceptor(m_main_loop, ip, port);                                                           // 创建连接接收器对象
    m_acceptor->setNewConnectionCallback(std::bind(&TCPServer::newConnection, this, std::placeholders::_1));    // 设置新连接回调函数

    m_thread_pool = new ThreadPool(thread_num, "IO"); // 创建线程池对象

    // 创建从事件循环
    for (int i = 0; i < thread_num; i++) {
        EventLoop *loop = new EventLoop();                                                                  // 创建从事件循环对象
        loop->setEpollTimeOutCallback(std::bind(&TCPServer::epollTimeOut, this, std::placeholders::_1));    // 设置epoll_wait超时回调函数
        m_thread_pool->addTask(std::bind(&EventLoop::run, loop));                                           // 启动从事件循环
        m_sub_loops.push_back(loop);                                                                        // 保存从事件循环对象
    }
}

// 析构函数
TCPServer::~TCPServer() {
    delete m_acceptor;                          // 释放监听套接字对象   
    delete m_main_loop;                         // 释放主事件循环对象
    for (auto &connection : m_connections) {    // 释放连接对象
        delete connection.second;
    }
    for (auto &loop : m_sub_loops) {            // 释放从事件循环对象
        delete loop;
    }
    delete m_thread_pool;                       // 释放线程池对象
}

// 启动服务器
void TCPServer::start() {
    m_main_loop->run();
}

// 处理新客户端连接请求
void TCPServer::newConnection(Socket *client_socket) {
    //Connection *connection = new Connection(m_main_loop, client_socket);                                // 创建连接对象(运行在主事件循环)
    Connection *connection = new Connection(m_sub_loops[client_socket->getFd()%m_thread_num], client_socket);   // 创建连接对象(运行在从事件循环)
    connection->setCloseCallback(std::bind(&TCPServer::closeConnection, this, std::placeholders::_1));  // 设置连接关闭回调函数
    connection->setErrorCallback(std::bind(&TCPServer::errorConnection, this, std::placeholders::_1));  // 设置连接异常回调函数
    connection->setHandleMessageCallback(std::bind(&TCPServer::handleMessage, this, std::placeholders::_1, std::placeholders::_2)); // 设置处理报文回调函数
    connection->setSendCompleteCallback(std::bind(&TCPServer::sendComplete, this, std::placeholders::_1));  // 设置发送完成回调函数

    //printf("新连接: fd %d, ip %s:%d\n", connection->getFd(), connection->getIp().c_str(), connection->getPort());

    m_connections.insert(std::make_pair(connection->getFd(), connection));  // 把连接对象添加到map中

    if(m_new_connection_callback)m_new_connection_callback(connection);     // 回调EchoServer::HandleNewConnection
}

// 关闭连接,在Connection类中回调
void TCPServer::closeConnection(Connection *connection) {
    if(m_close_connection_callback)m_close_connection_callback(connection);     // 回调EchoServer::HandleCloseConnection
    //printf("关闭连接: fd %d, ip %s:%d\n", connection->getFd(), connection->getIp().c_str(), connection->getPort());
    m_connections.erase(connection->getFd());   // 删除连接对象
    delete connection;                          // 释放连接对象
}

// 处理连接异常, 在Connection类中回调
void TCPServer::errorConnection(Connection *connection) {
    if (m_error_connection_callback)m_error_connection_callback(connection);    // 回调EchoServer::HandleErrorConnection
    //printf("连接异常: fd %d, ip %s:%d\n", connection->getFd(), connection->getIp().c_str(), connection->getPort());
    m_connections.erase(connection->getFd());   // 删除连接对象
    delete connection;                          // 释放连接对象
}

// 处理客户端请求报文,在Connection类中回调
void TCPServer::handleMessage(Connection *connection, std::string &message) {
    if(m_handle_message_callback)m_handle_message_callback(connection, message);    // 回调EchoServer::HandleMessage
}

// 处理连接发送完成, 在Channel类中回调
void TCPServer::sendComplete(Connection *connection) {
    //printf("发送完成: fd %d, ip %s:%d\n", connection->getFd(), connection->getIp().c_str(), connection->getPort());
    
    if(m_send_complete_callback)m_send_complete_callback(connection);   // 回调EchoServer::HandleSendComplete
}

// 处理epoll_wait超时, 在EventLoop类中回调
void TCPServer::epollTimeOut(EventLoop *loop) {
    //printf("epoll_wait超时\n");
    
    if(m_epoll_time_out_callback)m_epoll_time_out_callback(loop);   // 回调EchoServer::HandleEpollTimeOut
}

// 设置回调函数
void TCPServer::setNewConnectionCallback(std::function<void(Connection *)> callback) {
    m_new_connection_callback = callback;
}

void TCPServer::setCloseConnectionCallback(std::function<void(Connection *)> callback) {
    m_close_connection_callback = callback;
}

void TCPServer::setErrorConnectionCallback(std::function<void(Connection *)> callback) {
    m_error_connection_callback = callback;
}

void TCPServer::setHandleMessageCallback(std::function<void(Connection *, std::string&)> callback) {
    m_handle_message_callback = callback;
}

void TCPServer::setSendCompleteCallback(std::function<void(Connection *)> callback) {
    m_send_complete_callback = callback;
}

void TCPServer::setEpollTimeOutCallback(std::function<void(EventLoop *)> callback) {
    m_epoll_time_out_callback = callback;
}
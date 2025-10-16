#include "Connection.h"

// 构造函数
Connection::Connection(EventLoop *loop, Socket *client_socket) : m_loop(loop), m_client_socket(client_socket) {
    m_client_channel = new Channel(client_socket->getFd(), m_loop);                             // 创建clientfd的Channel对象
    m_client_channel->setReadCallback(std::bind(&Channel::handleMessage, m_client_channel));    // 设置读事件处理函数为处理对端发来的消息
    m_client_channel->setCloseCallback(std::bind(&Connection::close_callback, this));           // 设置关闭fd的回调函数
    m_client_channel->setErrorCallback(std::bind(&Connection::error_callback, this));           // 设置错误处理函数
    m_client_channel->setEdgeTriggered();                                                       // 设置为边缘触发
    m_client_channel->enableReading();                                                          // 将client的Channel对象设置为可读
}


// 析构函数
Connection::~Connection() {
    delete m_client_socket;
    delete m_client_channel;
}

// 获取client的fd
int Connection::getFd() const {
    return m_client_socket->getFd();
}

// 获取client的ip
std::string Connection::getIp() const {
    return m_client_socket->getIp();
}

// 获取client的端口号
uint16_t Connection::getPort() const {
    return m_client_socket->getPort();
}

// 关闭连接的回调函数,供Channel调用
void Connection::close_callback() {
    m_close_callback(this);
}

// 错误处理回调函数,供Channel调用
void Connection::error_callback() {
    m_error_callback(this);
}

// 设置关闭连接的回调函数
void Connection::setCloseCallback(std::function<void(Connection *)> callback) {
    m_close_callback = callback;
}

// 设置错误处理回调函数
void Connection::setErrorCallback(std::function<void(Connection *)> callback) {
    m_error_callback = callback;
}
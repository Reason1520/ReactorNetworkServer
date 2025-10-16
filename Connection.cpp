#include "Connection.h"

// 构造函数
Connection::Connection(EventLoop *loop, Socket *client_socket) : m_loop(loop), m_client_socket(client_socket) {
    m_client_channel = new Channel(client_socket->getFd(), m_loop);                             // 创建clientfd的Channel对象
    m_client_channel->enableReading();                                                          // 将client的Channel对象设置为可读
    m_client_channel->setEdgeTriggered();                                                       // 设置为边缘触发
    m_client_channel->setReadCallback(std::bind(&Channel::handleMessage, m_client_channel));    // 设置读事件处理函数为处理对端发来的消息
}


// 析构函数
Connection::~Connection() {
    delete m_client_socket;
    delete m_client_channel;
}
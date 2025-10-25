#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port, int sub_thread_num, int work_thread_num) 
    : m_tcp_server(ip, port, sub_thread_num), m_thread_pool(work_thread_num, "Work") {
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

// 停止服务
void EchoServer::Stop() {
    // 停止工作线程
    m_thread_pool.stop();
    printf("工作线程已停止\n");

    // 停止IO线程
    m_tcp_server.stop();
}

// 处理新客户端连接请求,在TCPServer类中回调
void EchoServer::HandleNewConnection(spConnection connection) {
    printf("new connection(fd = %d, ip = %s, port = %d) ok\n", connection->getFd(), connection->getIp().c_str(), connection->getPort());
    // printf("EchoServer::HandleNewConnection() thread is %ld.\n", syscall(SYS_gettid));
}

// 关闭客户端的连接,在TCPServer类中回调
void EchoServer::HandleCloseConnection(spConnection connection) {
    printf("close connection(fd = %d, ip = %s, port = %d) ok\n", connection->getFd(), connection->getIp().c_str(), connection->getPort());
}

// 客户端的连接错误,在TCPServer类中回调
void EchoServer::HandleErrorConnection(spConnection connection) {
    //std::cout << "Error Connection from " << connection->getIp() << ":" << connection->getPort() << std::endl;
}

// 处理客户端得请求报文,在TCPServer类中回调
void EchoServer::HandleMessage(spConnection connection, std::string &message) {
    //printf("EchoServer::HandleMessage() thread is %ld.\n", syscall(SYS_gettid));
    if (m_thread_pool.size() == 0) {    // 如果没有工作线程池,直接使用IO线程处理
        HandleMessage_thread(connection, message);
    }
    else {                              // 否则使用工作线程池处理
        m_thread_pool.addTask(std::bind(&EchoServer::HandleMessage_thread, this, connection, message)); // 把业务添加到线程池的任务队列中
    }
}

// 处理客户端得请求报文,用于添加给线程池
void EchoServer::HandleMessage_thread(spConnection connection, std::string &message) {
    //printf("EchoServer::HandleMessage_thread() thread is %ld.\n", syscall(SYS_gettid));
    message = "回复: " + message;         // 回显业务

    connection->send(message.data(), message.size()); // 把临时缓冲区的数据发送给客户端
}

// 数据发送完成,在TCPServer类中回调
void EchoServer::HandleSendComplete(spConnection connection) {
    //std::cout << "Send Complete to " << connection->getIp() << ":" << connection->getPort() << std::endl;
}

// epoll_wait超时,在TCPServer类中回调
void EchoServer::HandleEpollTimeOut(EventLoop *loop) {
    //std::cout << "Epoll Time Out" << std::endl;
}
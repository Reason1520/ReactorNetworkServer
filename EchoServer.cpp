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

// 处理新客户端连接请求,在TCPServer类中回调
void EchoServer::HandleNewConnection(Connection *connection) {
    std::cout << "New Connection from " << connection->getIp() << ":" << connection->getPort() << std::endl;
    printf("EchoServer::HandleNewConnection() thread is %ld.\n", syscall(SYS_gettid));
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
void EchoServer::HandleMessage(Connection *connection, std::string &message) {
    //printf("EchoServer::HandleMessage() thread is %ld.\n", syscall(SYS_gettid));

    // 把业务添加到线程池的任务队列中
    m_thread_pool.addTask(std::bind(&EchoServer::HandleMessage_thread, this, connection, message));
}

// 处理客户端得请求报文,用于添加给线程池
void EchoServer::HandleMessage_thread(Connection *connection, std::string &message) {
    //printf("EchoServer::HandleMessage_thread() thread is %ld.\n", syscall(SYS_gettid));
    message = "回复: " + message;         // 回显业务

    connection->send(message.data(), message.size()); // 把临时缓冲区的数据发送给客户端
}

// 数据发送完成,在TCPServer类中回调
void EchoServer::HandleSendComplete(Connection *connection) {
    std::cout << "Send Complete to " << connection->getIp() << ":" << connection->getPort() << std::endl;
}

// epoll_wait超时,在TCPServer类中回调
void EchoServer::HandleEpollTimeOut(EventLoop *loop) {
    //std::cout << "Epoll Time Out" << std::endl;
}
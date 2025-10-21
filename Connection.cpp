#include "Connection.h"
#include <cstring>

// 构造函数
Connection::Connection(const std::unique_ptr<EventLoop>& loop, std::unique_ptr<Socket> client_socket)
    : m_loop(loop), m_client_socket(std::move(client_socket)), m_diconnect(false)
{
    // 在将 client_socket 移动到成员后，必须使用成员 m_client_socket 来获取 fd，
    // 避免在初始化列表中访问已被 move 的参数导致未定义行为。
    m_client_channel = std::make_unique<Channel>(m_client_socket->getFd(), m_loop);
    m_client_channel->setReadCallback(std::bind(&Connection::handleMessage, this));     // 设置读事件处理函数为处理对端发来的消息
    m_client_channel->setCloseCallback(std::bind(&Connection::close_callback, this));   // 设置关闭fd的回调函数
    m_client_channel->setErrorCallback(std::bind(&Connection::error_callback, this));   // 设置错误处理函数
    m_client_channel->setWriteCallback(std::bind(&Connection::write_callback, this));   // 设置写事件处理函数为处理发送缓冲区中的数据
    m_client_channel->setEdgeTriggered();                                               // 设置为边缘触发
    m_client_channel->enableReading();                                                  // 将client的Channel对象设置为可读
}

// 析构函数
Connection::~Connection() {
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

// 处理对端发来的消息
void Connection::handleMessage() {
    char buffer[1024];
    while (true) { // 由于采用边缘触发, 所以可能会有数据未读完的情况, 需要循环读取
        memset(buffer, 0, sizeof(buffer));
        ssize_t ret = recv(this->getFd(), buffer, sizeof(buffer), 0);
        if (ret > 0) {                                                          // 如果有数据
            //printf("收到数据: fd: %d, data: %s\n", this->getFd(), buffer);
            //send(this->getFd(), buffer, strlen(buffer), 0); 
            m_input_buffer.append(buffer, ret); // 把读取到的数据追加到接收缓冲区中
        }
        else if (ret == -1 && errno == EINTR) {                                 // 如果是信号中断
            continue;
        }
        else if (ret == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 如果是数据已经读完
            //printf("收到数据: fd: %d, data: %s\n", this->getFd(), m_input_buffer.data());
            while (true) {
                int len;
                memcpy(&len, m_input_buffer.data(), 4);     // 从input_buffer中读取报文头部
                if (m_input_buffer.size() < len + 4) {      // 如果接收缓冲区中的数据长度小于报文头部长度,表示报文不完整,下次再读取
                    break;
                }

                std::string message(m_input_buffer.data() + 4, len);    // 从input_buffer中获取一个报文内容
                m_input_buffer.erase(0, len + 4);                       // 从input_buffer中删除已处理的数据

                printf("收到数据: fd: %d, data: %s\n", this->getFd(), message.c_str());

                m_handle_message_callback(shared_from_this(), message);   // 回调TCPServer::handleMessage
            }
            break;     
        }
        else if (ret == 0){                                                     // 如果是客户端连接断开
            close_callback();           // 调用关闭fd回调函数
            break;
        }
    }
}

// 关闭连接的回调函数,供Channel调用
void Connection::close_callback() {
    m_diconnect = true;         // 设置断开连接标志位
    m_client_channel->remove(); // 从事件循环中删除channel
    m_close_callback(shared_from_this());
}

// 错误处理回调函数,供Channel调用
void Connection::error_callback() {
    m_diconnect = true;         // 设置断开连接标志位
    m_client_channel->remove(); // 从事件循环中删除channel
    m_error_callback(shared_from_this());
}

// 处理写事件的回调函数,供Channel调用
void Connection::write_callback() {
    ssize_t ret = ::send(this->getFd(), m_output_buffer.data(), m_output_buffer.size(), 0); // 尝试把m_output_buffer中的数据发送给对端
    if (ret > 0) {
        m_output_buffer.erase(0, ret);          // 从发送缓冲区中删除已发送的数据
    }
    if (m_output_buffer.size() == 0) {
        m_client_channel->disableWriting();     // 如果发送缓冲区中没有数据,则取消注册写事件
        m_send_complete_callback(shared_from_this()); // 调用发送完成回调函数
    }
}

// 设置关闭连接的回调函数
void Connection::setCloseCallback(std::function<void(spConnection)> callback) {
    m_close_callback = callback;
}

// 设置错误处理回调函数
void Connection::setErrorCallback(std::function<void(spConnection)> callback) {
    m_error_callback = callback;
}

// 设置处理对端发送过来的数据的回调函数
void Connection::setHandleMessageCallback(std::function<void(spConnection, std::string&)> callback) {
    m_handle_message_callback = callback;
}

// 设置发送完成回调函数
void Connection::setSendCompleteCallback(std::function<void(spConnection)> callback) {
    m_send_complete_callback = callback;
}

// 发送数据
void Connection::send(const char *data, size_t size) {
    if (m_diconnect == true) {
        printf("Connection::send() error, connection has disconnect\n");
        return;
    }
    m_output_buffer.appendWithHead(data, size); // 把数据追加到Connection的发送缓冲区中
    m_client_channel->enableWriting();          // 注册写事件
}
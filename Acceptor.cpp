#include "Acceptor.h"

// 构造函数
Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port) : m_loop(loop)
{
    // 创建监听的socket
    m_listen_socket = new Socket(createNonblockingSocket());

    // 设置listenfd的属性
    m_listen_socket->setReuseaddr(true);  // 设置地址复用
    m_listen_socket->setTcpNoDelay(true); // 关闭Nagle算法
    m_listen_socket->setKeepalive(true);  // 开启心跳检测
    m_listen_socket->setReuseport(true);  // 端口复用

    // 绑定端口地址
    InetAddress server_addr(ip, port); // 创建InetAddress对象
    m_listen_socket->bindAddress(server_addr);

    // 监听端口
    m_listen_socket->listen();

    // 创建监听Channel
    m_listen_channel = new Channel(m_listen_socket->getFd(), loop);                     // 创建listenfd的Channel对象
    m_listen_channel->setReadCallback(std::bind(&Acceptor::handleNewConnection, this)); // Channel对象的回调函数为Acceptor对象的新连接处理函数
    m_listen_channel->enableReading();                                                  // 将listnefd的Channel对象设置为可读
}

// 析构函数
Acceptor::~Acceptor()
{
    delete m_listen_socket;
    delete m_listen_channel;
}

// 创建新的连接
void Acceptor::handleNewConnection()
{
    InetAddress client_addr;                                                    // 客户端的地址和协议信息
    Socket *client_socket = new Socket(m_listen_socket->accept(client_addr));   // 客户端的socket对象
    
    m_new_connection_callback(client_socket);                                   // 调用新连接回调函数,处理新连接
}

// 设置新连接回调函数
void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> callback) {
    m_new_connection_callback = callback;
}
#include "TCPServer.h"

// 构造函数
TCPServer::TCPServer(const std::string &ip, const uint16_t port) {
    // 创建监听的socket
    Socket *listen_socket = new Socket(createNonblockingSocket());

    // 设置listenfd的属性
    listen_socket->setReuseaddr(true);  // 设置地址复用
    listen_socket->setTcpNoDelay(true); // 关闭Nagle算法
    listen_socket->setKeepalive(true);  // 开启心跳检测
    listen_socket->setReuseport(true);  // 端口复用

    // 绑定端口地址
    InetAddress server_addr(ip, port); // 创建InetAddress对象
    listen_socket->bindAddress(server_addr);

    // 监听端口
    listen_socket->listen();

    // 创建监听Channel
    Channel *server_channel = new Channel(listen_socket->getFd(), &loop);                                       // 创建listenfd的Channel对象
    server_channel->setReadCallback(std::bind(&Channel::handleNewConnection, server_channel, listen_socket)); // Channel对象的回调函数为新连接处理函数
    server_channel->enableReading();                                                                           // 将listnefd的Channel对象设置为可读
}

// 析构函数
TCPServer::~TCPServer() {

}

void TCPServer::start() {
    loop.run();
}
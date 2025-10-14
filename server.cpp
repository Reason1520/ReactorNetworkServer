#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }

    // 创建监听的socket
    Socket listen_socket(createNonblockingSocket());

    // 设置listenfd的属性
    listen_socket.setReuseaddr(true);  // 设置地址复用
    listen_socket.setTcpNoDelay(true); // 关闭Nagle算法
    listen_socket.setKeepalive(true);  // 开启心跳检测
    listen_socket.setReuseport(true);  // 端口复用

    // 绑定端口地址
    InetAddress server_addr(argv[1], atoi(argv[2])); // 创建InetAddress对象
    listen_socket.bindAddress(server_addr);

    // 监听端口
    listen_socket.listen();

    // 设置epoll
    Epoll epoll;                                                                // 创建Epoll对象
    Channel *server_channel = new Channel(listen_socket.getFd(), &epoll); // 创建listenfd的Channel对象
    server_channel->setReadCallback(std::bind(&Channel::handleNewConnection, server_channel, &listen_socket)); // Channel对象的回调函数为新连接处理函数
    server_channel->enableReading();                                            // 将listnefd的Channel对象设置为可读

    // 循环处理事件
    while (true) {
        std::vector<Channel *> channels;    // 创建返回Channel数组指针
        channels = epoll.wait();            // 等待事件

        // 处理所有发生的事件
        for (auto &channel : channels) {
            channel->handleEvent();   // 处理事件
        }
    }
    return 0;
}
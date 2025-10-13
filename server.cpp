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
    Epoll epoll;                                 // 创建Epoll对象
    epoll.addFd(listen_socket.getFd(), EPOLLIN); // 将监听的文件描述符添加到epoll中
    std::vector<epoll_event> events;             // 创建返回事件数组指针

    // 循环处理事件
    while (true)
    {
        events = epoll.wait(); // 等待事件

        // 处理所有发生的事件
        for (auto &event : events)
        {
            if (event.events & EPOLLRDHUP)
            { // 如果是客户端关闭(或者recv返回0)
                printf("客户端关闭: fd %d\n", event.data.fd);
                close(event.data.fd);
            }
            else if (event.events & (EPOLLIN | EPOLLPRI))
            { // 如果是读事件(普通数据|带外数据)
                if (event.data.fd == listen_socket.getFd())
                {                                                                          // 如果是监听的文件描述符,表示新连接
                    InetAddress client_addr;                                               // 客户端的地址和协议信息
                    Socket *client_socket = new Socket(listen_socket.accept(client_addr)); // 客户端的socket对象

                    printf("新连接: fd %d, ip %s:%d\n", client_socket->getFd(), client_addr.getIp(), client_addr.getPort());

                    epoll.addFd(client_socket->getFd(), EPOLLIN | EPOLLET); // 添加新的文件描述符到epoll中
                }
                else
                { // 如果时客户端fd有事件,表示有数据可读
                    char buffer[1024];
                    while (true)
                    { // 由于采用边缘触发, 所以可能会有数据未读完的情况, 需要循环读取
                        memset(buffer, 0, sizeof(buffer));
                        ssize_t ret = recv(event.data.fd, buffer, sizeof(buffer), 0);
                        if (ret > 0)
                        { // 如果有数据
                            printf("收到数据: fd: %d, data: %s\n", event.data.fd, buffer);
                            send(event.data.fd, buffer, strlen(buffer), 0);
                        }
                        else if (ret == -1 && errno == EINTR)
                        { // 如果是信号中断
                            continue;
                        }
                        else if (ret == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
                        { // 如果是数据已经读完
                            break;
                        }
                        else if (ret == 0)
                        { // 如果是客户端连接断开
                            printf("客户端关闭: fd %d\n", event.data.fd);
                            close(event.data.fd);
                            break;
                        }
                    }
                }
            }
            else if (event.events & EPOLLOUT)
            { // 如果是写事件
            }
            else
            { // 其他事件(视为错误)
                printf("未知事件: fd %d, events %d\n", event.data.fd, event.events);
                close(event.data.fd);
            }
        }
    }
    return 0;
}
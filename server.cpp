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


int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }
    
    // 创建监听的socket
    Socket listen_socket(createNonblockingSocket());

    // 设置listenfd的属性
    listen_socket.setReuseaddr(true);   // 设置地址复用
    listen_socket.setTcpNoDelay(true);  // 关闭Nagle算法
    listen_socket.setKeepalive(true);   // 开启心跳检测
    listen_socket.setReuseport(true);   // 端口复用

    // 绑定端口地址
    InetAddress server_addr(argv[1], atoi(argv[2]));  // 创建InetAddress对象
    listen_socket.bindAddress(server_addr);

    // 监听端口
    listen_socket.listen();

    // 设置epoll
    int epollfd = epoll_create(1);                                      // 创建epoll句柄
    struct epoll_event event;                                           // 创建epoll事件结构体
    event.data.fd = listen_socket.getFd();                              // 设置监听的文件描述符
    event.events = EPOLLIN;                                             // 设置监听的事件类型(监听读事件)
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_socket.getFd(), &event);   // 将监听的文件描述符添加到epoll中
    struct epoll_event events[10];                                      // 创建返回事件数组

    // 循环处理事件
    while (true) { 
        int nfds = epoll_wait(epollfd, events, 10, -1);     // 等待事件发生

        if (nfds < 0) {
            perror("epoll_wait 失败");
            break;
        }
        if (nfds == 0) {
            printf("epoll_wait 超时\n");
            continue;
        }

        // 处理所有发生的事件
        for (int i = 0; i < nfds; i++) { 
            if (events[i].events & EPOLLRDHUP) {                    // 如果是客户端关闭(或者recv返回0)
                printf("客户端关闭: fd %d\n", events[i].data.fd);
                close(events[i].data.fd);
            }
            else if (events[i].events & (EPOLLIN | EPOLLPRI)) {     // 如果是读事件(普通数据|带外数据)
                if (events[i].data.fd == listen_socket.getFd()) {   // 如果是监听的文件描述符,表示新连接
                    InetAddress client_addr;                                                // 客户端的地址和协议信息
                    Socket *client_socket = new Socket(listen_socket.accept(client_addr));  // 客户端的socket对象

                    printf("新连接: fd %d, ip %s:%d\n", client_socket->getFd(), client_addr.getIp(), client_addr.getPort());

                    // 添加新的文件描述符到epoll中
                    event.data.fd = client_socket->getFd();                             // 设置新的文件描述符
                    event.events = EPOLLIN | EPOLLET;                                   // 设置监听的事件类型(边缘触发, 监听读事件)
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, client_socket->getFd(), &event);  // 将新的文件描述符添加到epoll中
                } 
                else {                                              // 如果时客户端fd有事件,表示有数据可读
                    char buffer[1024];
                    while (true) {      // 由于采用边缘触发, 所以可能会有数据未读完的情况, 需要循环读取
                        memset(buffer, 0, sizeof(buffer));
                        ssize_t ret = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                        if (ret > 0) {                                                          // 如果有数据
                            printf("收到数据: fd: %d, data: %s\n", events[i].data.fd, buffer);
                            send(events[i].data.fd, buffer, strlen(buffer), 0);
                        }
                        else if (ret == -1 && errno == EINTR) {                                 // 如果是信号中断
                            continue;
                        }
                        else if (ret == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 如果是数据已经读完
                            break;
                        }
                        else if (ret == 0) {                                                    // 如果是客户端连接断开
                            printf("客户端关闭: fd %d\n", events[i].data.fd);
                            close(events[i].data.fd);
                            break;
                        }
                    }
                }
            }
            else if (events[i].events & EPOLLOUT) {                 // 如果是写事件
            }
            else {                                                  // 其他事件(视为错误)
                printf("未知事件: fd %d, events %d\n", events[i].data.fd, events[i].events);
                close(events[i].data.fd);
            }
        }
    }
    return 0;
}
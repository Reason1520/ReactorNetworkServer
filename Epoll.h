#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <vector>

class Epoll
{
private:
    int epollfd = -1;                      // epoll文件描述符
    static const int MAX_EVENTS = 1024;    // 最大监听事件数
    struct epoll_event events[MAX_EVENTS]; // 存放epoll返回的所有事件

public:
    Epoll();
    ~Epoll();

    void getFd();                                       // 获取epollfd
    void addFd(int fd, uint32_t op);                    // 添加事件fd
    std::vector<epoll_event> wait(int timeout = -1);    // 等待事件
};
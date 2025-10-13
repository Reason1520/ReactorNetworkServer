#include "Epoll.h"

// 构造函数
Epoll::Epoll()
{
    epollfd = epoll_create(1); // 创建epoll句柄
    if (epollfd == -1)
    {
        perror("epoll_create");
        exit(-1);
    }
}

// 析构函数
Epoll::~Epoll()
{
    close(epollfd);
}

// 添加事件
void Epoll::addFd(int fd, uint32_t op)
{
    struct epoll_event event; // 创建epoll事件结构体
    event.data.fd = fd;       // 设置监听的文件描述符
    event.events = op;        // 设置监听的事件类型(监听读事件)
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) < 0)
    { // 将监听的文件描述符添加到epoll中
        perror("epoll_ctl");
        exit(-1);
    }
}

// 等待事件
std::vector<epoll_event> Epoll::wait(int timeout)
{
    bzero(events, sizeof(events));
    int nfds = epoll_wait(epollfd, events, 10, -1); // 等待事件发生
    if (nfds < 0)
    {
        perror("epoll_wait 失败");
        exit(-1);
    }
    if (nfds == 0)
    {
        printf("epoll_wait 超时\n");
        exit(-1);
    }

    std::vector<epoll_event> ret;
    for (int i = 0; i < nfds; i++)
    {
        ret.push_back(events[i]);
    }
    return ret;
}
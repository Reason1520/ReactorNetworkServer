#include "Epoll.h"
#include "Channel.h"

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

// 把channel添加/更新到红黑树中
void Epoll::updateChannel(Channel *channel) {
    struct epoll_event event;               // 创建epoll事件结构体
    event.data.ptr = channel;               // 设置监听的文件描述符
    event.events = channel->getEvents();    // 设置监听的事件类型(监听读事件)
    if (channel->isEpollIn()) {     // channel在epoll树中
        if (epoll_ctl(epollfd, EPOLL_CTL_MOD, channel->getFd(), &event) < 0) {
            perror("epoll_ctl");
            exit(-1);
        }
    }
    else {                          // channel不在epoll树中
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, channel->getFd(), &event) < 0) {
            perror("epoll_ctl");
            exit(-1);
        }
        channel->setEpollIn(true);  // 将channel设置为在epoll树中
    }
}

// 等待事件
std::vector<Channel *> Epoll::wait(int timeout)
{
    bzero(events, sizeof(events));
    std::vector<Channel *> channels;                // 创建返回的vector
    int nfds = epoll_wait(epollfd, events, 10, -1); // 等待事件发生
    if (nfds < 0) {     //返回失败
        perror("epoll_wait 失败");
        exit(-1);
    }
    if (nfds == 0) {    // 返回超时   
        printf("epoll_wait 超时\n");
        return channels;
    }

    for (int i = 0; i < nfds; i++)
    {
        Channel *channel = (Channel *)events[i].data.ptr;   // 取出已发生事件的channel
        channel->setRevents(events[i].events);              // 设置channel的revent
        channels.push_back(channel);
    }
    return channels;
}
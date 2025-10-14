#pragma once
#include <sys/epoll.h>
#include <functional>
#include "Socket.h"
#include "Epoll.h"
#include "InetAddress.h"

class Channel
{
private:
    int m_fd;                        // Channel拥有的fd
    struct Epoll *m_epoll = nullptr; // 携带此Channel的epoll_event所在的epoll红黑树
    bool m_isEpollIn = false;        // 携带此Channel的epoll_event是否在epoll红黑树中
    uint32_t m_events = 0;           // 监听的事件(listnenfd监听EPOLLIN, clientfd监听EPOLLIN | EPOLLOUT)
    uint32_t m_revents = 0;          // 已经触发的事件
    std::function<void()> m_read_callback;  // 读事件回调函数

public:
    Channel(int fd, Epoll *epoll);   // 构造函数
    ~Channel();                                     // 析构函数
    int getFd();                                    // 获取fd
    bool isEpollIn();                               // 获取是否在epoll红黑树中
    uint32_t getEvents();                           // 获取监听的事件
    uint32_t getRevents();                          // 获取已经触发的事件
    void setEpollIn(bool isEpollIn);                // 设置是否在epoll红黑树中
    void setEvents(uint32_t events);                // 设置监听的事件
    void setRevents(uint32_t revents);              // 设置已经触发的事件
    void setEdgeTriggered();                        // 设置边缘触发
    void enableReading();                           // 让epoll_wait()监听m_fd的读事件
    void handleEvent();                             // 事件处理函数,epoll_wait()返回时执行

    void handleNewConnection(Socket *listen_socket);    // 处理新客户端连接请求
    void handleMessage();                               // 处理对端发来的消息
    void setReadCallback(std::function<void()> callback);   // 设置读事件回调函数
};

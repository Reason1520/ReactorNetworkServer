#pragma once
#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/epoll.h>
#include <functional>
#include <string>

class Socket;
class EventLoop;

class Channel
{
private:
    int m_fd = -1;                          // Channel拥有的fd
    EventLoop *m_eventloop = nullptr;       // 运行此Channel的eventloop
    bool m_isEpollIn = false;               // 携带此Channel的epoll_event是否在epoll红黑树中
    uint32_t m_events = 0;                  // 监听的事件(listnenfd监听EPOLLIN, clientfd监听EPOLLIN | EPOLLOUT)
    uint32_t m_revents = 0;                 // 已经触发的事件
    std::function<void()> m_read_callback;  // 读事件回调函数
    std::function<void()> m_close_callback; // 关闭fd回调函数,调Connection::m_cloes_callback()
    std::function<void()> m_error_callback; // 错误回调函数,调Connection::m_error_callback()

public:
    Channel(int fd, EventLoop *eventloop);          // 构造函数
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

    void handleMessage();                                   // 处理对端发来的消息
    void setReadCallback(std::function<void()> callback);   // 设置读事件回调函数
    void setCloseCallback(std::function<void()> callback);  // 设置关闭fd回调函数
    void setErrorCallback(std::function<void()> callback);  // 设置错误回调函数
};

#endif

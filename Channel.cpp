#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Connection.h"
#include <cstring>

// 构造函数
Channel::Channel(int fd, EventLoop *eventloop) : m_fd(fd), m_eventloop(eventloop) {
}

// 析构函数
Channel::~Channel() {}

// 获取fd
int Channel::getFd() {
    return m_fd;
}

// 获取是否在epoll红黑树中
bool Channel::isEpollIn() {
    return m_isEpollIn;
}

// 获取监听的事件
uint32_t Channel::getEvents() {
    return m_events;
}

// 获取已经触发的事件
uint32_t Channel::getRevents() {
    return m_revents;
}

// 设置是否在epoll红黑树中
void Channel::setEpollIn(bool isEpollIn) {
    m_isEpollIn = isEpollIn;
}

// 设置监听的事件
void Channel::setEvents(uint32_t events) {
    m_events = events;
}

// 设置已经触发的事件
void Channel::setRevents(uint32_t revents) {
    m_revents = revents;
}

// 设置边缘触发
void Channel::setEdgeTriggered() {
    m_events = m_events | EPOLLET;
}

// 设置EPOLLOUT(让epoll_wait()监听m_fd的读事件)
void Channel::enableReading() {
    m_events = m_events | EPOLLIN;      // 设置监听EPOLLIN
    m_eventloop->updateChannel(this);   // 更新epoll_wait()监听的fd
}

// 事件处理函数,epoll_wait()返回时执行
void Channel::handleEvent() {
    if (m_revents & EPOLLRDHUP) {                   // 如果是客户端关闭(或者recv返回0)
        m_close_callback(); // 调用关闭fd回调函数
    }
    else if (m_revents & (EPOLLIN | EPOLLPRI)) {    // 如果是读事件(普通数据|带外数据)
        m_read_callback();  // 调用读事件处理函数
    }
    else if (m_revents & EPOLLOUT) {                // 如果是写事件
    }
    else {                                          // 其他事件(视为错误)
        m_error_callback(); // 调用错误回调函数
    }
}

// 处理对端发来的消息
void Channel::handleMessage() {
    char buffer[1024];
    while (true)
    { // 由于采用边缘触发, 所以可能会有数据未读完的情况, 需要循环读取
        memset(buffer, 0, sizeof(buffer));
        ssize_t ret = recv(m_fd, buffer, sizeof(buffer), 0);
        if (ret > 0) {                                                          // 如果有数据
            printf("收到数据: fd: %d, data: %s\n", m_fd, buffer);
            send(m_fd, buffer, strlen(buffer), 0);
        }
        else if (ret == -1 && errno == EINTR) {                                 // 如果是信号中断
            continue;
        }
        else if (ret == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 如果是数据已经读完
            break;
        }
        else if (ret == 0) {                                                    // 如果是客户端连接断开
            m_close_callback(); // 调用关闭fd回调函数
            break;
        }
    }
}

// 设置读事件回调函数
void Channel::setReadCallback(std::function<void()> callback) {
    m_read_callback = callback;
}

// 设置关闭fd回调函数
void Channel::setCloseCallback(std::function<void()> callback) {
    m_close_callback = callback;
}

// 设置错误回调函数
void Channel::setErrorCallback(std::function<void()> callback) {
    m_error_callback = callback;
}
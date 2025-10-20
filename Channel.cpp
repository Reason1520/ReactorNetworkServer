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

// 取消读事件
void Channel::disableReading() {
    m_events = m_events & (~EPOLLIN);   // 取消监听EPOLLIN
    m_eventloop->updateChannel(this);   // 更新epoll_wait()监听的fd
}

// 注册写事件
void Channel::enableWriting() {
    m_events = m_events | EPOLLOUT;     // 监听EPOLLOUT
    m_eventloop->updateChannel(this);   // 更新epoll_wait()监听的fd
}

// 取消写事件
void Channel::disableWriting() {
    m_events = m_events & (~EPOLLOUT);  // 取消监听EPOLLOUT
    m_eventloop->updateChannel(this);   // 更新epoll_wait()监听的fd
}

// 禁用所有事件
void Channel::disableAll() {
    m_events = 0;
    m_eventloop->updateChannel(this);
}

// 删除Channel
void Channel::remove() {
    disableAll();
    m_eventloop->removeChannel(this);
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
        m_write_callback(); // 调用写事件处理函数
    }
    else {                                          // 其他事件(视为错误)
        m_error_callback(); // 调用错误回调函数
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

// 设置写事件回调函数
void Channel::setWriteCallback(std::function<void()> callback) {
    m_write_callback = callback;
}
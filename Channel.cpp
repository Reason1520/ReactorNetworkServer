#include "Channel.h"

// 构造函数
Channel::Channel(int fd, Epoll *epoll) : m_fd(fd), m_epoll(epoll)
{
}

// 析构函数
Channel::~Channel() {}

// 获取fd
int Channel::getFd()
{
    return m_fd;
}

// 获取是否在epoll红黑树中
bool Channel::isEpollIn()
{
    return m_isEpollIn;
}

// 获取监听的事件
uint32_t Channel::getEvents()
{
    return m_events;
}

// 获取已经触发的事件
uint32_t Channel::getRevents()
{
    return m_revents;
}

// 设置是否在epoll红黑树中
void Channel::setEpollIn(bool isEpollIn)
{
    m_isEpollIn = isEpollIn;
}

// 设置监听的事件
void Channel::setEvents(uint32_t events)
{
    m_events = events;
}

// 设置已经触发的事件
void Channel::setRevents(uint32_t revents)
{
    m_revents = revents;
}

// 设置边缘触发
void Channel::setEdgeTriggered()
{
    m_events = m_events | EPOLLET;
}

// 设置EPOLLOUT(让epoll_wait()监听m_fd的读事件)
void Channel::enableReading()
{
    m_events = m_events | EPOLLIN;
    m_epoll->updateChannel(this);
}
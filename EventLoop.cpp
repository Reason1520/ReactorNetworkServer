#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"

// 构造函数
EventLoop::EventLoop() : m_epoll(new Epoll) {

}

// 析构函数
EventLoop::~EventLoop() {
    delete m_epoll; 
}

// 运行循环
void EventLoop::run() {
    while (true) {
        std::vector<Channel *> channels;    // 创建返回Channel数组指针
        channels = m_epoll->wait(10 *1000);         // 等待事件

        if (channels.size() == 0) { //如果channels为空，表示超时,则回调TCPServer::epollTimeOut()
            m_epoll_time_out_callback(this);
        }

        // 处理所有发生的事件
        for (auto &channel : channels) {
            channel->handleEvent(); // 处理事件
        }
    }
}

// 获取epoll对象
Epoll *EventLoop::getEpoll() {
    return m_epoll;
}

// 更新channel
void EventLoop::updateChannel(Channel *channel) {
    m_epoll->updateChannel(channel);
}

// 设置epoll_wait()超时回调函数
void EventLoop::setEpollTimeOutCallback(std::function<void(EventLoop *)> callback) {
    m_epoll_time_out_callback = callback;
}
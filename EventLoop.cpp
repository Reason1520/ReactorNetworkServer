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
        channels = m_epoll->wait();         // 等待事件

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
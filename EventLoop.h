#pragma once
#include <functional>
#include <unistd.h>
#include <syscall.h>
#include <memory>

class Channel;
class Epoll;

class EventLoop {
private:
    std::unique_ptr<Epoll> m_epoll;     // 循环对应的唯一Epoll
    std::function<void(EventLoop *)> m_epoll_time_out_callback; // epoll_wait()超时回调函数
public:
    EventLoop();        // 构造函数
    ~EventLoop();       // 析构函数

    void run();         // 运行循环

    void updateChannel(Channel *channel);   // 更新channel
    void removeChannel(Channel *channel);   // 删除channel
    void setEpollTimeOutCallback(std::function<void(EventLoop *)> callback);    // 设置epoll_wait()超时回调函数
};

#pragma once

class Channel;
class Epoll;

class EventLoop {
private:
    Epoll *m_epoll;     // 循环对应的唯一Epoll
public:
    EventLoop();        // 构造函数
    ~EventLoop();       // 析构函数

    void run();         // 运行循环
    Epoll *getEpoll();  // 获取epoll
    void updateChannel(Channel *channel);   // 更新channel
};

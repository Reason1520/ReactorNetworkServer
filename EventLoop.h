#pragma once
#include <functional>
#include <unistd.h>
#include <syscall.h>
#include <memory>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>

class Channel;
class Epoll;

class EventLoop {
private:
    std::unique_ptr<Epoll> m_epoll;                             // 循环对应的唯一Epoll
    std::function<void(EventLoop *)> m_epoll_time_out_callback; // epoll_wait()超时回调函数
    pid_t m_thread_id;                                          // 事件循环线程id(IO线程)
    int m_wakeup_fd;                                            // 唤醒IO线程的fd
    std::unique_ptr<Channel> m_wakeup_channel;                  // 唤醒IO线程的channel
    std::mutex m_mutex;                                         // 任务队列互斥锁
    std::queue<std::function<void()>> m_task_queue;             // 任务队列
public:
    EventLoop();        // 构造函数
    ~EventLoop();       // 析构函数

    void run();         // 运行循环

    void updateChannel(Channel *channel);   // 更新channel
    void removeChannel(Channel *channel);   // 删除channel
    void setEpollTimeOutCallback(std::function<void(EventLoop *)> callback);    // 设置epoll_wait()超时回调函数
    bool is_in_loop_thread();                                                   // 判断当前线程是否是loop线程
    void addTask(std::function<void()> task);                                   // 添加任务到队列中
    void wakeup();                                                              // 唤醒事件循环线程
    void handleWakeup();                                                        // 处理唤醒事件
};

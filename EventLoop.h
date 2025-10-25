#pragma once
#include <functional>
#include <unistd.h>
#include <syscall.h>
#include <memory>
#include <queue>
#include <mutex>
#include <map>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <atomic>
#include "Connection.h"

class Connection;
class Channel;
class Epoll;
using spConnection = std::shared_ptr<Connection>;

class EventLoop {
private:
    std::unique_ptr<Epoll> m_epoll;                             // 循环对应的唯一Epoll
    std::function<void(EventLoop *)> m_epoll_time_out_callback; // epoll_wait()超时回调函数
    pid_t m_thread_id;                                          // 事件循环线程id(IO线程)
    int m_wakeup_fd;                                            // 唤醒IO线程的fd
    std::unique_ptr<Channel> m_wakeup_channel;                  // 唤醒IO线程的channel
    int m_timer_fd;                                             // 定时器fd
    std::unique_ptr<Channel> m_timer_channel;                   // 定时器channel
    std::mutex m_taskqueue_mutex;                               // 任务队列互斥锁
    std::queue<std::function<void()>> m_task_queue;             // 任务队列
    bool m_is_mainloop;                                         // 是否是主循环
    std::map<int, spConnection> m_connections_map;              // 存储了连接了fd,Connection
    std::mutex m_connections_mutex;                             // 保护 m_connections_map 的互斥锁
    std::function<void(int)> m_connection_timeout_callback;     // 连接超时回调,删除TCPServer超时CConnection对象
    int m_timer_interval;                                       // 定时器间隔 
    int m_timeout;                                              // 连接超时时间
    std::atomic_bool m_stop;                                    // 停止标志

public:
    EventLoop(bool mainloop, int time_interval = 30, int timeout = 80);   // 构造函数
    ~EventLoop();               // 析构函数

    void run();                 // 运行循环
    void stop();                // 停止循环

    void updateChannel(Channel *channel);   // 更新channel
    void removeChannel(Channel *channel);   // 删除channel
    void setEpollTimeOutCallback(std::function<void(EventLoop *)> callback);    // 设置epoll_wait()超时回调函数
    bool is_in_loop_thread();                                                   // 判断当前线程是否是loop线程
    void addTask(std::function<void()> task);                                   // 添加任务到队列中
    void wakeup();                                                              // 唤醒事件循环线程
    void handleWakeup();                                                        // 处理唤醒事件
    void handleTimer();                                                         // 处理定时器事件
    void newConnection(spConnection connection);                                // 把Connection对象保存在m_connections_map中
    void setConnectionTimeoutCallback(std::function<void(int)> callback);       // 设置连接超时回调函数
};

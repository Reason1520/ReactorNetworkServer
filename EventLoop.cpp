#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"

// 构造函数
EventLoop::EventLoop() 
    : m_epoll(new Epoll), m_wakeup_fd(eventfd(0, EFD_NONBLOCK)), m_wakeup_channel(new Channel(m_wakeup_fd, this)) {
    m_wakeup_channel->setReadCallback(std::bind(&EventLoop::handleWakeup, this));   // 设置唤醒回调函数
    m_wakeup_channel->enableReading();
}

// 析构函数
EventLoop::~EventLoop() {
}

// 运行循环
void EventLoop::run() {
    //printf("EventLoop::run() thread is %ld\n", syscall(SYS_gettid));
    m_thread_id = syscall(SYS_gettid);      // 获取当前线程ID
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

// 更新channel
void EventLoop::updateChannel(Channel *channel) {
    m_epoll->updateChannel(channel);
}

// 删除channel
void EventLoop::removeChannel(Channel *channel) {
    m_epoll->removeChannel(channel);
}

// 设置epoll_wait()超时回调函数
void EventLoop::setEpollTimeOutCallback(std::function<void(EventLoop *)> callback) {
    m_epoll_time_out_callback = callback;
}

// 判断当前线程是否为事件循环线程
bool EventLoop::is_in_loop_thread() {
    return m_thread_id == syscall(SYS_gettid);
}

// 添加任务到队列中
void EventLoop::addTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);  // 锁
        m_task_queue.push(task);                    // 任务入队
    }
    // 唤醒事件循环线程
}

// 唤醒事件循环线程
void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(m_wakeup_fd, &one, sizeof(one));  // 向eventfd中写入数据
}

// 处理唤醒事件
void EventLoop::handleWakeup() {
    printf("handleWakeup() thread is %ld\n", syscall(SYS_gettid)); 
    uint64_t one;
    read(m_wakeup_fd, &one, sizeof(one));       // 读取eventfd中的数据

    // 执行任务队列中的全部任务
    std::lock_guard<std::mutex> lock(m_mutex);  // 给任务队列加锁
    std::function<void()> task;
    while (m_task_queue.size() > 0) {
        task = std::move(m_task_queue.front()); // 获取任务
        m_task_queue.pop();                     // 删除任务
        task();                                 // 执行任务
    }
    
}
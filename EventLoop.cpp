#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"

// 创建定时器
int createtimefd(int sec = 5)
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    struct itimerspec timeout;            // 定时器结构体
    memset(&timeout, 0, sizeof(timeout)); // 初始化结构体
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd, 0, &timeout, 0); // 设置定时器
    return timerfd;
}

// 构造函数
EventLoop::EventLoop(bool mainloop, int time_interval, int timeout)
    : m_epoll(new Epoll), m_wakeup_fd(eventfd(0, EFD_NONBLOCK)), m_wakeup_channel(new Channel(m_wakeup_fd, this)),
    m_timer_fd(createtimefd(timeout)), m_timer_channel(new Channel(m_timer_fd, this)), m_is_mainloop(mainloop),
    m_timer_interval(time_interval), m_timeout(timeout) {
    m_wakeup_channel->setReadCallback(std::bind(&EventLoop::handleWakeup, this));   // 设置唤醒回调函数
    m_wakeup_channel->enableReading();
    m_timer_channel->setReadCallback(std::bind(&EventLoop::handleTimer, this));     // 设置定时器回调函数
    m_timer_channel->enableReading();
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
        std::lock_guard<std::mutex> lock(m_taskqueue_mutex);  // 锁
        m_task_queue.push(task);                    // 任务入队
    }
    // 唤醒事件循环线程
    wakeup();
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
    std::lock_guard<std::mutex> lock(m_taskqueue_mutex);  // 给任务队列加锁
    std::function<void()> task;
    while (m_task_queue.size() > 0) {
        task = std::move(m_task_queue.front()); // 获取任务
        m_task_queue.pop();                     // 删除任务
        task();                                 // 执行任务
    } 
}

// 处理定时器事件
void EventLoop::handleTimer() {
    // 重新计时
    struct itimerspec timeout;            // 定时器结构体
    memset(&timeout, 0, sizeof(timeout)); // 初始化结构体
    timeout.it_value.tv_sec = m_timer_interval;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(m_timer_fd, 0, &timeout, 0); // 设置定时器

    if (m_is_mainloop) {
        //printf("主事件循环的闹钟时间到了\n");
    }
    else {
        //printf("从事件循环的闹钟时间到了\n");
        printf("EventLoop::handleTimer() thread is %ld ", syscall(SYS_gettid));
        time_t now = time(0); // 获取当前时间
        for (auto it = m_connections_map.begin(); it != m_connections_map.end();) { // 遍历所有连接(迭代器安全写法)
            printf("%d", it->first);
            if (it->second->isTimeout(now, m_timeout)) {
                int fd = it->first;
                auto conn = it->second;
                {
                    std::lock_guard<std::mutex> lock(m_connections_mutex);
                    it = m_connections_map.erase(it);
                }
                conn->close_callback();
                m_connection_timeout_callback(fd);
            } else {
                ++it;
            }
        }
        printf("\n");
    }
}

// 把Connection对象保存在m_connections_map中
void EventLoop::newConnection(spConnection connection) {
    {
        std::lock_guard<std::mutex> lock(m_connections_mutex);
        m_connections_map[connection->getFd()] = connection;
    }
}

// 设置连接超时回调函数
void EventLoop::setConnectionTimeoutCallback(std::function<void(int)> callback) {
    m_connection_timeout_callback = callback;
}
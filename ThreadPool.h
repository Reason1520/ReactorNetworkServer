#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <sys/syscall.h>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <atomic>


class ThreadPool { 
private:
    std::vector<std::thread> m_threads;         // 线程池中的线程
    std::queue<std::function<void()>> m_tasks;  // 任务队列
    std::mutex m_mutex;                         // 任务队列的互斥锁
    std::condition_variable m_condition;        // 任务队列的条件变量
    std::atomic_bool m_stop;                    // 线程池的停止信号,若设置未true,则全部线程退出
public:
    ThreadPool(size_t thread_num = 4);          // 在构造函数中启动thhread_num个线程
    ~ThreadPool();                              // 析构函数中停止所有线程
    void addTask(std::function<void()> task);   // 添加任务到队列中
};
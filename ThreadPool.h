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
    std::string m_thread_type;                  // 线程类型,有两种取值"IO"和"Work"
public:
    ThreadPool(size_t thread_num, const std::string &threadtype);   // 在构造函数中启动thhread_num个线程
    ~ThreadPool();                                                  // 析构函数中停止所有线程
    void addTask(std::function<void()> task);                       // 添加任务到队列中
    ssize_t size() const;                                           // 返回线程池的大小
    void stop();                                                    // 停止所有线程
};
#include "ThreadPool.h"

// 在构造函数中启动thhread_num个线程
ThreadPool::ThreadPool(size_t thread_num, const std::string& threadtype) :m_stop(false), m_thread_type(threadtype) {
    for (int i = 0; i < thread_num; i++) {  // 创建thread_num个线程
        m_threads.emplace_back([this]{  // 使用lambda表达式创建线程
            printf("creat %s thread %ld\n",m_thread_type.c_str(), syscall(SYS_gettid));   // 获取当前线程ID
            while (this->m_stop == false) {
                std::function<void()> task;
                {   // 锁的作用域
                    std::unique_lock<std::mutex> lock(this->m_mutex);
                    m_condition.wait(lock, [this]{                  // 等待生产者的条件变量
                        return (this->m_stop == true) || (this->m_tasks.empty() == false);  // 线程池停止且任务队列为空时返回true,
                    });
                    if (this->m_stop && this->m_tasks.empty()) {    // 线程池停止之前,如果队列中还有任务,执行完再退出
                        return;
                    }
                    task = std::move(this->m_tasks.front());    // 出队一个元素
                    this->m_tasks.pop();                        // 从队头中删除
                }

                //printf("%s(%ld) execute task\n", m_thread_type.c_str(), syscall(SYS_gettid));
                task(); // 执行任务
            }
        });
    }
}

// 析构函数中停止所有线程
ThreadPool::~ThreadPool() {
    stop();
}

// 添加任务到队列中
void ThreadPool::addTask(std::function<void()> task) {
    {   // 锁的作用域
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push(task); // 入队
    }
    m_condition.notify_one(); // 唤醒一个等待的工作线程，执行新任务
}

// 返回线程池的大小
ssize_t ThreadPool::size() const {
    return m_threads.size();
}

// 停止线程池
void ThreadPool::stop() {
    if (m_stop) return;
    m_stop = true;
    m_condition.notify_all(); // 唤醒所有线程

    for (auto &thread : m_threads) { // 等全部线程执行完后退出
        thread.join();
    }
}
#include "Channel.h"

// 构造函数
Channel::Channel(int fd, Epoll *epoll) : m_fd(fd), m_epoll(epoll) {
}

// 析构函数
Channel::~Channel() {}

// 获取fd
int Channel::getFd() {
    return m_fd;
}

// 获取是否在epoll红黑树中
bool Channel::isEpollIn() {
    return m_isEpollIn;
}

// 获取监听的事件
uint32_t Channel::getEvents() {
    return m_events;
}

// 获取已经触发的事件
uint32_t Channel::getRevents() {
    return m_revents;
}

// 设置是否在epoll红黑树中
void Channel::setEpollIn(bool isEpollIn) {
    m_isEpollIn = isEpollIn;
}

// 设置监听的事件
void Channel::setEvents(uint32_t events) {
    m_events = events;
}

// 设置已经触发的事件
void Channel::setRevents(uint32_t revents) {
    m_revents = revents;
}

// 设置边缘触发
void Channel::setEdgeTriggered() {
    m_events = m_events | EPOLLET;
}

// 设置EPOLLOUT(让epoll_wait()监听m_fd的读事件)
void Channel::enableReading() {
    m_events = m_events | EPOLLIN;
    m_epoll->updateChannel(this);
}

// 事件处理函数,epoll_wait()返回时执行
void Channel::handleEvent() {
    if (m_revents & EPOLLRDHUP) {                   // 如果是客户端关闭(或者recv返回0)
        printf("客户端关闭: fd %d\n", this->getFd());
        close(this->getFd());
    }
    else if (m_revents & (EPOLLIN | EPOLLPRI)) {    // 如果是读事件(普通数据|带外数据)
        m_read_callback();    // 调用读事件处理函数
    }
    else if (m_revents & EPOLLOUT) {                // 如果是写事件
    }
    else {                                          // 其他事件(视为错误)
        printf("未知事件: fd %d, events %d\n", m_fd, m_revents);
        close(m_fd);
    }
}

// 处理新客户端连接请求
void Channel::handleNewConnection(Socket *listen_socket) {
    InetAddress client_addr;                                                        // 客户端的地址和协议信息
    Socket *client_socket = new Socket(listen_socket->accept(client_addr));         // 客户端的socket对象

    printf("新连接: fd %d, ip %s:%d\n", client_socket->getFd(), client_addr.getIp(), client_addr.getPort());

    Channel *client_channel = new Channel(client_socket->getFd(), m_epoll);  // 创建clientfd的Channel对象
    client_channel->enableReading();                                                // 将listnefd的Channel对象设置为可读
    client_channel->setEdgeTriggered();                                             // 设置为边缘触发
    client_channel->setReadCallback(std::bind(&Channel::handleMessage, client_channel));      // 设置读事件处理函数为处理对端发来的消息
}

// 处理对端发来的消息
void Channel::handleMessage() {
    char buffer[1024];
    while (true)
    { // 由于采用边缘触发, 所以可能会有数据未读完的情况, 需要循环读取
        memset(buffer, 0, sizeof(buffer));
        ssize_t ret = recv(m_fd, buffer, sizeof(buffer), 0);
        if (ret > 0) {                                                          // 如果有数据
            printf("收到数据: fd: %d, data: %s\n", m_fd, buffer);
            send(m_fd, buffer, strlen(buffer), 0);
        }
        else if (ret == -1 && errno == EINTR) {                                 // 如果是信号中断
            continue;
        }
        else if (ret == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 如果是数据已经读完
            break;
        }
        else if (ret == 0) {                                                    // 如果是客户端连接断开
            printf("客户端关闭: fd %d\n", m_fd);
            close(m_fd);
            break;
        }
    }
}

// 设置读事件回调函数
void Channel::setReadCallback(std::function<void()> callback) {
    m_read_callback = callback;
}
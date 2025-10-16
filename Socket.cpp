#include "Socket.h"

// 创建非阻塞socket
int createNonblockingSocket() {
    int listenfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);   //SOCK_NONBLOCK:创建非阻塞socket
    if (listenfd == -1) {
        perror("listen socket 创建失败");
        exit(-1);
    }
    return listenfd;
}

// 构造函数
Socket::Socket(int fd) : m_fd(fd) {}

// 析构函数
Socket::~Socket() {
    close(m_fd);
}

// 获取fd
int Socket::getFd() const {
    return m_fd;
}

// 获取ip
std::string Socket::getIp() const {
    return m_ip;
}

// 获取端口
uint16_t Socket::getPort() const {
    return m_port;
}

// 设置地址复用
void Socket::setReuseaddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval)));
}

// 设置端口复用
void Socket::setReuseport(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval)));
}

// 设置心跳检测
void Socket::setKeepalive(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof(optval)));
}

// 禁用Nagle算法
void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof(optval)));
}

// 绑定地址
void Socket::bindAddress(const InetAddress &server_addr) {
    if (::bind(m_fd, server_addr.getSockAddr(), server_addr.getSockLen()) < 0) {
        perror("bind 绑定失败");
        close(m_fd);
        exit(-1);
    }
    m_ip = server_addr.getIp();     // 设置ip
    m_port = server_addr.getPort(); // 设置端口
}

// 监听
void Socket::listen(int size) {
    if (::listen(m_fd, size) < 0) {
        perror("listen 监听失败");
        close(m_fd);
        exit(-1);
    }
}

// 接收连接
int Socket::accept(InetAddress &client_addr) {
    struct sockaddr_in pre_addr;                                                            // 客户端地址结构体
    socklen_t pre_addrlen = sizeof(pre_addr);                                               // 客户端地址长度
    int connfd = accept4(m_fd, (struct sockaddr *)&pre_addr, &pre_addrlen, SOCK_NONBLOCK);  // 接受连接,非阻塞
    client_addr.setAddr(pre_addr);                                                          // 设置客户端地址

    // 有问题,后面修
    //m_ip = client_addr.getIp();     // 设置ip
    //m_port = client_addr.getPort(); // 设置端口

    return connfd;
}
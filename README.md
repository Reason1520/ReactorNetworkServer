# Reactor 网络服务器

## 项目描述
基于 Reactor 模式的高性能网络服务器

## 文件说明
- `server.cpp` - 服务器主程序
- `client.cpp` - 客户端测试程序
- `InetAddress.h/cpp` - 网络地址类,处理地址相关
- `Socket.h/cpp` - 网络套接字类,用于封装socket
- `Epoll.h/cpp` - Epoll类,用于封装epoll
- `Channel.h/cpp` - 通道类,用于封装文件描述符和事件,并进行事件处理;随epoll_event传递
- `makefile` - 编译配置
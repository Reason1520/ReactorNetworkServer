# Reactor 网络服务器

## 项目描述
基于 Reactor 模式的高性能网络服务器

## 文件说明
- `server.cpp` - 服务器主程序
- `client.cpp` - 客户端测试程序(手动输入)
- `client2.cpp` - 客户端测试程序(自动输入)
- `InetAddress.h/cpp` - 网络地址类,处理地址相关
- `Socket.h/cpp` - 网络套接字类,用于封装socket
- `Epoll.h/cpp` - Epoll类,用于封装epoll
- `Channel.h/cpp` - 通道类,用于封装文件描述符和事件,并进行事件处理;随epoll_event传递
- `EventLoop.h/cpp` - 事件循环类,用于处理事件
- `Acceptor.h/cpp` - 接收器类,用于接收客户端连接请求
- `Connetion.h/cpp` - 连接类,用于运行与客户端的连接
- `Buffer.h/cpp` - 缓冲区类,用于存储数据
- `TimeStamp.h/cpp` - 定时器类,用于管理定时,提供时间服务
- `TCPSServer.h/cpp` - 使用TCP协议实现的服务器类
- `EchoServer.h/cpp` - 回显服务器类,业务层
- `makefile` - 编译配置
- `Instruction` - 项目结构描述
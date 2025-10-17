# ReactorNetworkServer — 架构说明（Instruction 目录）

本文档基于当前仓库源码（`Channel`, `EventLoop`, `Epoll`, `TCPServer`, `Socket`, `InetAddress`, `server.cpp`, `client.cpp`）整理，目的是为阅读者快速理解模块关系、运行时流程与如何可视化项目结构。

## 概览

- `server.cpp`：程序入口，创建 `TCPServer` 并调用 `start()`。
- `TCPServer`：创建非阻塞监听 `Socket`，为监听 fd 创建 `Channel` 并绑定 `EventLoop`。
- `EventLoop`：持有 `Epoll`，在 `run()` 循环中通过 `Epoll::wait()` 获取活跃 `Channel*` 并调用 `Channel::handleEvent()`。
- `Epoll`：封装 epoll 系统调用，负责将 `Channel` 注册到 epoll（add/mod）并在 `wait()` 中把活跃事件映射回 `Channel*`。
- `Channel`：封装单个 fd 的事件与回调（例如监听 fd 的 `handleNewConnection`，客户 fd 的 `handleMessage`）。
- `Socket`：封装 socket 系统操作（bind/listen/accept/setsockopt），提供非阻塞 socket 创建函数 `createNonblockingSocket()`。
- `InetAddress`：封装 `sockaddr_in`（IP/port）。
 - `Acceptor`：负责监听套接字的创建与 accept 流程，封装监听 `Socket` 与监听 `Channel`，通过回调把新连接交给 `TCPServer`。
 - `Connection`：代表一个已建立的客户端连接，维护客户端 `Socket` 与对应的 `Channel`，负责为客户端 Channel 绑定读回调（`handleMessage`）。

## 模块关系（ASCII）

ServerApp (server.cpp)
  |
  v
TCPServer
  |-- owns --> Socket (listen socket)
  |-- creates --> Channel (server_channel)
  |-- creates --> Acceptor (m_acceptor)

EventLoop
  |-- owns --> Epoll
  |-- calls --> Epoll::wait() -> returns list<Channel*>

Epoll
  |-- manages --> Channel (epoll fd registration)

Channel
  |-- wraps --> fd (listen or client)
  |-- callbacks:
        - handleNewConnection (for listen fd)
        - handleMessage (for client fd)
  |-- interacts --> Socket (accept -> new client socket)

Acceptor
  |-- owns --> Socket (listen socket)
  |-- creates --> Channel (listen channel)

Connection
  |-- owns --> Socket (client socket)
  |-- creates --> Channel (client channel)

Socket
  |-- operations --> bind/listen/accept

InetAddress
  |-- used-by --> Socket / Channel

## 典型运行时流程

- 新连接：epoll_wait -> EventLoop (获得 Channel*) -> Channel::handleEvent() -> handleNewConnection() -> Socket::accept() -> new client Socket + new client Channel -> client Channel.enableReading() -> EventLoop/Epoll 注册
- 客户端消息：epoll_wait -> EventLoop -> client Channel.handleEvent() -> handleMessage() -> recv/send（回显）

## diagram.dot（Graphviz）

仓库根目录下存在 `diagram.dot`（基于源码生成的类/模块关系）。你可以在仓库根目录运行以下命令来渲染图像：

```bash
# 生成 PNG
dot -Tpng diagram.dot -o diagram.png

# 生成 SVG
dot -Tsvg diagram.dot -o diagram.svg
```
# ReactorNetworkServer — 架构说明（Instruction 目录）

本文档基于当前仓库源码（`Channel`, `EventLoop`, `Epoll`, `TCPServer`, `Acceptor`, `Connection`, `Socket`, `InetAddress`, `server.cpp`, `client.cpp` 等）整理，目标是精确反映当前实现、模块关系、运行时流程以及如何可视化、渲染项目结构。

## 快速索引

- 主要源文件：
  - `Acceptor.h/.cpp` — 监听套接字管理与 accept 流程
  - `Channel.h/.cpp` — fd 封装与事件回调
  - `Connection.h/.cpp` — 表示客户端连接，维护缓冲区与回调
  - `TCPServer.h/.cpp` — 服务器入口逻辑，管理 `Acceptor` 与连接集合
  - `EventLoop.h/.cpp`, `Epoll.h/.cpp` — 事件循环与 epoll 封装
  - `Socket.h/.cpp`, `InetAddress.h/.cpp` — socket 与地址封装
  - `Buffer.h/.cpp` — I/O 缓冲区（Connection 使用）
  - `server.cpp`, `client.cpp` — 示例服务与客户端

## 模块概览（更完整）

- `server.cpp`：程序入口，创建 `TCPServer` 并调用 `start()`。
- `TCPServer`：管理 `EventLoop`、`Acceptor` 和所有活跃 `Connection`（保存在 `m_connections` map 中）。提供一系列回调接口：`setNewConnectionCallback`, `setCloseConnectionCallback`, `setErrorConnectionCallback`, `setHandleMessageCallback`, `setSendCompleteCallback`, `setEpollTimeOutCallback`，用于上层（例如 EchoServer）注册业务逻辑。
- `Acceptor`：负责监听 fd 的创建、bind/listen、accept；当 accept 到新连接时创建客户端 `Socket` 并通过 `m_new_connection_callback` 将新连接交给 `TCPServer`。
- `Connection`：封装客户端连接，包括 `Socket`, `Channel`, 输入/输出 `Buffer`（`m_input_buffer`, `m_output_buffer`），并暴露多个回调：close/error/handle_message/send_complete。`Connection` 会为 client fd 创建 `Channel` 并设置为可读、边沿触发（EPOLLET）。
- `Channel`：与 epoll 交互的薄封装，维护 fd、events/revents、以及 read 回调。`Channel::handleEvent()` 根据 revents 分发到 `m_read_callback`（或处理关闭）
- `EventLoop` / `Epoll`：`EventLoop::run()` 循环中调用 `Epoll::wait()` 获取活跃 `Channel*`，并逐个调用 `Channel::handleEvent()`。
- `Socket` / `InetAddress`：底层系统调用的封装，提供非阻塞 socket 的创建与 accept（accept4(..., SOCK_NONBLOCK)）。
- `Buffer`：用于 Connection 的输入输出缓冲，帮助处理边沿触发下的分块读取/写入。

## ASCII 模块关系

ServerApp (server.cpp)
  |
  v
TCPServer
  |-- owns --> EventLoop (m_loop)
  |-- owns --> Acceptor (m_acceptor)
  |-- manages --> Connection map (m_connections)

Acceptor
  |-- owns --> Socket (listen socket)
  |-- creates --> Channel (listen channel)
  `-- on accept -> TCPServer::newConnection(Socket*)

Connection
  |-- owns --> Socket (client socket)
  |-- creates --> Channel (client channel)
  |-- holds --> Buffer (input/output)
  |-- callbacks --> close / error / handle_message / send_complete

EventLoop
  |-- owns --> Epoll
  |-- run() -> Epoll::wait() -> list<Channel*> -> Channel::handleEvent()

Epoll
  |-- manages --> Channel (epoll_ctl add/mod)

Channel
  |-- wraps --> fd
  |-- dispatches --> read callback / close

Socket
  |-- operations --> bind/listen/accept

InetAddress
  |-- used-by --> Socket / Connection

## 关键运行序列（细化）

1. 启动：`main` 创建 `TCPServer`，调用 `start()` -> `m_loop.run()` 开始事件循环。
2. 监听注册：`TCPServer` 在构造中创建 `Acceptor`，`Acceptor` 创建监听 `Socket` 和 `Channel`，并将 `listen fd` 注册到 `EventLoop`/`Epoll`。
3. 新连接到达：`epoll_wait` 返回 listen channel，`Channel::handleEvent()` 调用 `Acceptor::handleNewConnection()` -> `Socket::accept()` 创建 client fd 并 new 出 `Socket*` -> `m_new_connection_callback(client_socket)` 调用 `TCPServer::newConnection(Socket*)`。
4. 建立 Connection：`TCPServer::newConnection` 创建 `Connection`，`Connection` 创建 client `Channel`（设置为可读/边沿触发）、设置回调并把 connection 存入 `m_connections`（key 为 fd）。同时 `TCPServer` 会将若干业务回调绑定到 `Connection`（close/error/handle_message/send_complete）。
5. 客户端数据流：client Channel 被 epoll 触发 -> `Channel::handleEvent()` 调用 `Connection::handleMessage()`（通过回调） -> Connection 从 `Socket` `recv()` 到 `m_input_buffer`，处理业务（通过 `m_handle_message_callback` 回调把消息转给上层如 EchoServer），并把响应放入 `m_output_buffer`，再通过 `send()`/写事件完成发送，最终调用 `m_send_complete_callback`。
6. 关闭/错误：当客户端断开或发生错误，`Connection` 调用其 `m_close_callback`/`m_error_callback`，上层 `TCPServer` 从 `m_connections` 中删除并 delete connection。

## diagram（更新说明）

我已更新 `Instruction/diagram.dot` 以反映当前源码结构（添加 `EchoServer`, `Buffer` 节点，明晰 TCPServer 与 EventLoop/Acceptor/Connection 的关系，以及回调链路）。 要渲染或重新生成图像，请在 `Instruction` 目录或仓库根目录运行下面的命令。

在 `Instruction` 目录运行（输出到该目录）：

```bash
# 生成 PNG（输出为 Instruction/diagram.png）
dot -Tpng Instruction/diagram.dot -o Instruction/diagram.png

# 生成 SVG（输出为 Instruction/diagram.svg）
dot -Tsvg Instruction/diagram.dot -o Instruction/diagram.svg
```

或者在仓库根目录运行（等同，只是路径写法不同）：

```bash
dot -Tpng Instruction/diagram.dot -o Instruction/diagram.png
dot -Tsvg Instruction/diagram.dot -o Instruction/diagram.svg
```

如果你希望把生成的图片放到仓库根目录以便 README 直接引用，可以运行：

```bash
dot -Tpng Instruction/diagram.dot -o diagram.png
```

在你的 Markdown 查看器或 GitHub 上，使用相对路径引用图像，例如：

![Architecture diagram](Instruction/diagram.png)

## 注意与改进建议

- 内存管理：代码中使用 `new`/`delete`（`Acceptor`, `Connection`, `Channel`, `Socket` 等），建议引入 `std::unique_ptr` / `std::shared_ptr` 并明确所有权边界。
- 错误与资源清理：当前在多处直接 `perror` 后 `exit(-1)`，建议替换为可回收的错误路径或异常，以便优雅关闭并释放资源。
- 日志：用可配置的日志库替换 `printf`；添加连接/断开/错误/超时等日志。
- 回调设计：当前回调链已足够表达基本回显逻辑，若引入复杂协议，考虑把回调参数标准化（例如传入 `Connection*` 与 `Buffer&`）。

## 变更摘要（本次更新）

- 更新 `Instruction/diagram.dot`：
  - 新增 `EchoServer` 节点以表示业务层。
  - 明确 `Connection` 使用 `Buffer`，并列出回调（onMessage/onClose/onError/onWriteComplete）。
  - 在 runtime cluster 中聚合 `EventLoop`/`Epoll`/`Channel`/`Connection`/`Buffer`，便于理解运行时事件流。

- 更新 `Instruction/ARCHITECTURE.md` 的 diagram 渲染说明，添加如何在本地生成 PNG/SVG 并把图片移动到根目录供 README 使用的说明。

如果你希望我替你执行下一步（选项之一）：

- 把这些改动提交到 git（我可以运行 `git add`/`git commit -m "chore: update architecture docs"` 并推送），或
- 运行 dot 将 `Instruction/diagram.dot` 渲染为 PNG/SVG，并把图片复制到仓库根目录并在 `README.md` 中添加引用。

告诉我你要哪个，我会接着做。

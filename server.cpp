#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "TCPServer.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }

    // 创建服务器
    TCPServer server(argv[1], atoi(argv[2]));

    // 启动
    server.start();
    return 0;
}
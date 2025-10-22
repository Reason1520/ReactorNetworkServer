#include "EchoServer.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }

    // 创建回显服务器
    EchoServer server(argv[1], atoi(argv[2]), 3, 0);

    // 启动
    server.Start();
    return 0;
}
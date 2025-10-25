#include "EchoServer.h"
#include <signal.h>

EchoServer *echoserver;

void sig_handler(int sig)
{
    printf("receive signal %d\n", sig);
    echoserver->Stop();
    delete echoserver;
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }

    signal(SIGTERM, sig_handler);   // 信号15,系统kill或killall命令默认发送的信号
    signal(SIGINT, sig_handler);    // 信号2,Ctrl+C

    // 创建回显服务器
    echoserver = new EchoServer(argv[1], atoi(argv[2]), 3, 2);

    // 启动
    echoserver->Start();
    return 0;
}
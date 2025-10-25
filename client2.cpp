#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket creat error");
        return -1;
    }

    // 设置地址信息
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // 连接服务器
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        printf("connect (%s:%s) error", argv[1], argv[2]);
        close(sockfd);
        return -1;
    }
    printf("connect (%s:%s) success\n", argv[1], argv[2]);
    printf("开始事件: %ld\n", time(0));

    char buf[1024];
    // 发送数据
    for (int i = 0; i < 100000; i++)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "这是第%d个超级女生", i);

        char tempbuf[1024];                 // 临时缓冲区,报文头部+报文内容
        memset(tempbuf, 0, sizeof(tempbuf));
        int len = strlen(buf);              // 报文长度
        memcpy(tempbuf, &len, 4);           // 拼接报文头部
        memcpy(tempbuf + 4, buf, len);      // 拼接报文内容

        // 发送数据
        send(sockfd, tempbuf, len + 4, 0);

        // 接收数据
        recv(sockfd, &len, 4, 0);           // 接收报文长度

        memset(buf, 0, sizeof(buf));
        recv(sockfd, buf, len, 0);          // 接收报文内容

        // printf("recv: %s\n", buf);
    }
    printf("结束时间: %ld\n", time(0));
}
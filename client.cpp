#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[]) { 
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
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
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        printf("connect (%s:%s) error", argv[1], argv[2]);
        close(sockfd);
        return -1;
    }
    printf("connect (%s:%s) success\n", argv[1], argv[2]);

    // 发送数据
    char buf[1024];
    while (true) { 
        // 输入数据
        memset(buf, 0, sizeof(buf));
        printf("input(q to quit): ");
        scanf("%s", buf);
        if (strcmp(buf, "q") == 0) {
            break;
        }

        // 发送数据
        if (send(sockfd, buf, strlen(buf), 0) == -1) {
            perror("send error");
            close(sockfd);
            return -1;
        }

        // 接收数据
        memset(buf, 0, sizeof(buf));
        if (recv(sockfd, buf, sizeof(buf), 0) == -1) {
            perror("recv error");
            close(sockfd);
            return -1;
        }
        printf("recv: %s\n", buf);
    }
}
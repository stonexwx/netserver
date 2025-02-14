// 网络通讯的客户端程序。
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
        printf("usage:./client ip port\n");
        printf("example:./client 192.168.150.128 5085\n\n");
        return -1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() failed.\n");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connect(%s:%s) failed.\n", argv[1], argv[2]);
        close(sockfd);
        return -1;
    }

    printf("connect ok.\n");
    printf("开始时间：%ld\n", time(0));
    for (int ii = 0; ii < 1000000; ii++)
    {
        // 从命令行输入内容。
        memset(buf, 0, sizeof(buf));
        // printf("please input:");
        // scanf("%s", buf);
        sprintf(buf, "hello world %d", ii);

        char tmpbuf[1024];
        memset(tmpbuf, 0, sizeof(tmpbuf));

        int len = strlen(buf);
        memcpy(tmpbuf, &len, 4);
        memcpy(tmpbuf + 4, buf, len);

        if (send(sockfd, tmpbuf, len + 4, 0) <= 0) // 把命令行输入的内容发送给服务端。
        {
            printf("write() failed.\n");
            close(sockfd);
            return -1;
        }
        recv(sockfd, &len, 4, 0);
        memset(buf, 0, sizeof(buf));
        recv(sockfd, buf, len, 0);
        // printf("recv:%s\n", buf);
    }
    printf("结束时间：%ld\n", time(0));
}
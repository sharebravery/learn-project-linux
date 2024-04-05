#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE 256

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server_address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int c_fd;
    char buf[MAX_LINE];
    struct sockaddr_in client_addr;
    int len, bytes_sent;

    if ((c_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket error");
        exit(1);
    }

    // 初始化服务器地址结构
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(argv[1]);
    client_addr.sin_port = htons(atoi(argv[2]));

    int status = connect(c_fd, (struct sockaddr *)&client_addr, sizeof(client_addr));
    if (status == -1)
    {
        perror("Connect error");
        close(c_fd);
        exit(1);
    }

    printf("已连接到服务器，端口号：%s\n", argv[2]);

    while (1)
    {
        // 发送数据到服务器
        printf("请输入要发送给服务器的消息（输入 quit 退出）：");
        fgets(buf, MAX_LINE, stdin);

        if (strcmp(buf, "quit\n") == 0)
            break;

        bytes_sent = send(c_fd, buf, strlen(buf), 0); // 发送实际接收到的字节数
        if (bytes_sent == -1)
        {
            perror("Send error");
            close(c_fd);
            exit(1);
        }
        printf("发送 %d 字节到服务器\n", bytes_sent);

        // 接收来自服务器的回复
        len = recv(c_fd, buf, MAX_LINE, 0);
        if (len == -1)
        {
            perror("Receive error");
            close(c_fd);
            exit(EXIT_FAILURE);
        }

        buf[len] = '\0';
        printf("接收到服务器的回复：%s", buf);
    }

    // 关闭套接字
    close(c_fd);

    return 0;
}

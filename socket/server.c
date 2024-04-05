#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PADDING 10
#define MAX_LINE 256

void handle_client(int c_fd, struct sockaddr_in client_addr);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server_address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int s_fd;

    struct sockaddr_in server_addr;

    int len;

    s_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (s_fd == -1)
    {
        perror("Socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    printf("server address: %s:%s\n", argv[1], argv[2]);

    int status = bind(s_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (status == -1)
    {
        perror("Bind error");
        close(s_fd);
        exit(1);
    }

    if (listen(s_fd, MAX_PADDING) == -1)
    {
        perror("Listen error");
        close(s_fd);
        exit(1);
    }

    printf("服务器等待连接，端口号：%s...\n", argv[2]);

    // 接收来自客户端的连接
    while (1)
    {
        struct sockaddr_in client_addr;
        int c_fd;
        len = sizeof(client_addr);

        c_fd = accept(s_fd, (struct sockaddr *)&client_addr, &len);

        if (c_fd == -1)
        {
            perror("Accept error");
            close(s_fd);
            exit(1);
        }

        printf("连接成功，客户端地址：%s，端口号：%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // 创建子进程处理客户端请求
        pid_t pid = fork();
        if (pid == 0)
        {
            // 子进程处理客户端请求
            close(s_fd); // 关闭监听套接字，子进程不需要它
            handle_client(c_fd, client_addr);
            exit(0); // 处理完客户端请求后子进程退出
        }
        else if (pid < 0)
        {
            perror("Fork error");
            close(c_fd);
            close(s_fd);
            exit(1);
        }
        else
        {
            // 父进程关闭客户端套接字，继续等待新的连接
            close(c_fd);
        }
    }

    close(s_fd);

    return 0;
}

void handle_client(int c_fd, struct sockaddr_in client_addr)
{
    char buf[MAX_LINE];
    int bytes_received;

    while (1)
    {
        // 接受来自客户端的数据
        bytes_received = recv(c_fd, buf, MAX_LINE, 0);
        if (bytes_received > 0)
        {
            buf[bytes_received] = '\0';
            printf("接收到的消息：%s", buf);

            // 回显数据给客户端
            if (send(c_fd, buf, bytes_received, 0) == -1)
            {
                perror("Send error");
                close(c_fd);
                exit(1);
            }
        }
        else if (bytes_received == 0) // 客户端关闭连接
        {
            printf("客户端已关闭连接\n");
            close(c_fd);
            break;
        }
        else
        {
            perror("Receive error");
            close(c_fd);
            exit(EXIT_FAILURE);
        }
    }
}

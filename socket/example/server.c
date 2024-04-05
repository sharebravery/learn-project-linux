#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_PENDING 5
#define MAX_LINE 256

int main()
{
    struct sockaddr_in server_addr, client_addr;
    char buf[MAX_LINE];
    int server_sock, client_sock;
    int len, bytes_received;

    // 创建套接字
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket error");
        exit(1);
    }

    // 初始化服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 将服务器的 IP 地址设置为本地主机的 IP 地址，使用 INADDR_ANY 表示任意可用的 IP 地址
    server_addr.sin_port = htons(PORT);              // 使用 htons 函数将端口号从主机字节序转换为网络字节序

    // 绑定套接字
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind error");
        close(server_sock);
        exit(1);
    }

    // 监听连接请求
    if (listen(server_sock, MAX_PENDING) == -1)
    {
        perror("Listen error");
        close(server_sock);
        exit(1);
    }

    printf("服务器等待连接，端口号：%d...\n", PORT);

    // 接受连接
    len = sizeof(client_addr);
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &len)) == -1)
    {
        perror("Accept error");
        close(server_sock);
        exit(1);
    }

    printf("连接成功，客户端地址：%s，端口号：%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // 接收来自客户端的数据
    while ((bytes_received = recv(client_sock, buf, MAX_LINE, 0)) > 0)
    {
        buf[bytes_received] = '\0';
        printf("接收到消息：%s", buf);

        // 回显数据给客户端
        if (send(client_sock, buf, bytes_received, 0) == -1)
        {
            perror("Send error");
            close(client_sock);
            close(server_sock);
            exit(1);
        }
    }

    if (bytes_received == -1)
    {
        perror("Receive error");
    }

    // 关闭套接字
    close(client_sock);
    close(server_sock);

    return 0;
}

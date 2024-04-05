#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_LINE 256

int main()
{
    struct sockaddr_in server_addr;
    char buf[MAX_LINE];
    int client_sock;
    int len, bytes_sent;

    // 创建套接字
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket error");
        exit(1);
    }

    // 初始化服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // 连接服务器
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connect error");
        close(client_sock);
        exit(1);
    }

    printf("已连接到服务器，端口号：%d\n", PORT);

    // 发送数据到服务器
    printf("请输入要发送给服务器的消息：");
    fgets(buf, MAX_LINE, stdin);

    if ((bytes_sent = send(client_sock, buf, strlen(buf), 0)) == -1)
    {
        perror("Send error");
        close(client_sock);
        exit(1);
    }

    printf("发送 %d 字节到服务器\n", bytes_sent);

    // 接收来自服务器的回复
    len = recv(client_sock, buf, MAX_LINE, 0);
    if (len == -1)
    {
        perror("Receive error");
        close(client_sock);
        exit(1);
    }

    buf[len] = '\0';
    printf("接收到服务器的回复：%s", buf);

    // 关闭套接字
    close(client_sock);

    return 0;
}

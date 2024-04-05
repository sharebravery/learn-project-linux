#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

// 定义命令类型的常量
#define LS 0
#define PWD 1
#define GET 2
#define IFGO 3
#define CD 4
#define PUT 5
#define LLS 6
#define LCD 7
#define LPWD 8
#define QUIT 9
#define DOFILE 10

// 定义消息结构体
struct Msg
{
    int type;            // 命令类型
    char cmd[1024];      // 命令内容
    char secondBuf[128]; // 第二个缓冲区
};

// 从命令字符串中获取目录部分
char *getdir(char *cmd)
{
    char *p = NULL;
    p = strtok(cmd, " ");
    p = strtok(NULL, " ");
    return p;
}

// 获取命令的类型
int getCmdType(char *cmd)
{
    // 判断命令类型并返回对应的常量
    if (!strcmp("ls", cmd))
        return LS;
    if (!strcmp("lls", cmd))
        return LLS;
    if (!strcmp("pwd", cmd))
        return PWD;
    if (!strcmp("quit", cmd))
        return QUIT;

    if (strstr(cmd, "cd"))
        return CD;
    if (strstr(cmd, "get"))
        return GET;
    if (strstr(cmd, "put"))
        return PUT;

    return -1;
}

// 处理客户端发送的消息
void msg_handler(struct Msg msg, int fd)
{
    char *file = NULL;
    char dataBuf[1024] = {0};
    int fdFile;

    int ret = getCmdType(msg.cmd);
    switch (ret)
    {
    case LS:
    case PWD:
        // 执行ls、pwd命令并将结果写入msg.cmd中，然后发送给客户端
        msg.type = 0;
        FILE *r = popen(msg.cmd, "r");
        fread(msg.cmd, sizeof(msg.cmd), 1, r);
        write(fd, &msg, sizeof(msg));
        break;
    case CD:
        // 执行cd命令并切换目录
        msg.type = 1;
        char *dir = getdir(msg.cmd);
        printf("dir: %s\n", dir);
        chdir(dir);
        break;
    case GET:
        // 处理get命令，获取文件内容并发送给客户端
        file = getdir(msg.cmd);
        if (access(file, F_OK) == -1)
        {
            strcpy(msg.cmd, "No This file!");
            write(fd, &msg, sizeof(msg));
        }
        else
        {
            msg.type = DOFILE;
            fdFile = open(file, O_RDWR);
            read(fdFile, dataBuf, sizeof(dataBuf));
            close(fdFile);
            strcpy(msg.cmd, dataBuf);
            write(fd, &msg, sizeof(msg));
        }
        break;
    case PUT:
        // 处理put命令，将客户端发送的内容写入文件中
        fdFile = open(getdir(msg.cmd), O_RDWR | O_CREAT, 0666);
        write(fdFile, msg.secondBuf, strlen(msg.secondBuf));
        close(fdFile);
        break;
    case QUIT:
        // 处理quit命令，退出服务器程序
        printf("client quit!\n");
        exit(-1);
    }
}

// 主函数
int main(int argc, char **argv)
{
    int s_fd;   // 服务器socket描述符
    int c_fd;   // 客户端socket描述符
    int n_read; // 读取的字节数

    struct sockaddr_in s_addr; // 服务器地址信息
    struct sockaddr_in c_addr; // 客户端地址信息
    struct Msg msg;            // 消息结构体

    // 参数检查
    if (argc != 3)
    {
        printf("param is not good\n");
        exit(-1);
    }

    memset(&s_addr, 0, sizeof(struct sockaddr_in));

    // 1. 创建socket
    s_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s_fd == -1)
    {
        perror("socket");
        exit(-1);
    }

    // 2. 绑定地址
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &s_addr.sin_addr);
    bind(s_fd, (struct sockaddr *)&s_addr, sizeof(struct sockaddr_in));

    // 3. 监听
    listen(s_fd, 10);

    // 4. 接受连接请求并处理
    int clen = sizeof(struct sockaddr_in);
    while (1)
    {
        c_fd = accept(s_fd, (struct sockaddr *)&c_addr, &clen);
        if (c_fd == -1)
        {
            perror("accept");
        }
        printf("get connect :%s\n", inet_ntoa(c_addr.sin_addr));

        // 创建子进程处理客户端命令
        if (fork() == 0)
        {
            while (1)
            {
                memset(msg.cmd, 0, sizeof(msg.cmd));
                // 5. 读取客户端发送的消息
                n_read = read(c_fd, &msg, sizeof(msg));
                if (n_read == 0)
                {
                    printf("client out\n");
                    break;
                }
                else if (n_read > 0)
                {
                    // 6. 处理消息并返回结果给客户端
                    msg_handler(msg, c_fd);
                }
            }
        }
    }
    // 7. 关闭socket
    close(s_fd);
    close(c_fd);
    return 0;
}

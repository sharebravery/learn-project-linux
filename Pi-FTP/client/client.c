#include <stdio.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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
    int type;
    char cmd[1024];
    char seconBuf[1280];
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

// 处理命令
int cmd_handler(struct Msg msg, int c_fd)
{
    int ret;
    char buf[32] = {0};
    int filefd;
    char *dir = NULL;

    ret = getCmdType(msg.cmd);
    switch (ret)
    {
    case LS:
    case CD:
    case PWD:
        // 发送命令给服务器，并等待服务器返回结果
        msg.type = 0;
        printf("msg: %s\n", msg.cmd);
        write(c_fd, &msg, sizeof(msg));
        break;
    case GET:
        // 发送get命令给服务器，并等待服务器返回文件内容
        msg.type = 2;
        write(c_fd, &msg, sizeof(msg));
        break;
    case PUT:
        // 发送put命令和文件内容给服务器
        strcpy(buf, msg.cmd);
        dir = getdir(buf);
        if (access(dir, F_OK) == -1)
        {
            printf("%s not exsit\n", dir);
        }
        else
        {
            filefd = open(dir, O_RDWR);
            read(filefd, msg.seconBuf, sizeof(msg.seconBuf));
            close(filefd);

            write(c_fd, &msg, sizeof(msg));
        }
        break;
    case LLS:
        // 执行本地ls命令
        system("ls");
        break;
    case LCD:
        // 切换本地目录
        dir = getdir(msg.cmd);
        chdir(dir);
        break;
    case QUIT:
        // 发送quit命令给服务器
        strcpy(msg.cmd, "quit");
        write(c_fd, &msg, sizeof(msg));
        close(c_fd);
        exit(-1);
    }
    return ret;
}

// 服务器消息处理函数
void serverMsgHandler(struct Msg msg, int c_fd)
{
    int n_read;
    struct Msg msgGet;
    char *dir = NULL;
    int newFile;

    // 读取服务器返回的消息
    n_read = read(c_fd, &msgGet, sizeof(msgGet));
    if (n_read == 0)
    {
        printf("server is out, quit\n");
        exit(-1);
    }
    else if (msgGet.type == DOFILE)
    {
        // 将服务器发送的文件内容写入本地文件
        dir = getdir(msg.cmd);
        newFile = open(dir, O_RDWR | O_CREAT, 0600);
        write(newFile, msgGet.cmd, strlen(msgGet.cmd));
        putchar('>');
        fflush(stdout);
    }
    else
    {
        // 打印服务器返回的消息
        printf("==========================\n");
        printf("\n%s\n", msgGet.cmd);
        printf("===========================\n");

        putchar('>');
        fflush(stdout);
    }
}

// 主函数
int main(int argc, char **argv)
{
    int c_fd;
    int ret;

    struct sockaddr_in c_addr;
    struct Msg msg;

    // 参数检查
    if (argc != 3)
    {
        printf("param is not good\n");
        exit(-1);
    }

    // 1. 创建socket
    c_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (c_fd == -1)
    {
        perror("socket");
        exit(-1);
    }

    // 2. 连接服务器
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &c_addr.sin_addr);

    int con = connect(c_fd, (struct sockaddr *)&c_addr, sizeof(struct sockaddr));
    if (con == -1)
    {
        perror("connect");
        exit(-1);
    }
    printf("connect---------------------\n");
    int mark = 0;
    // 3. 与服务器通信
    while (1)
    {
        memset(msg.cmd, 0, sizeof(msg.cmd));
        if (mark == 0)
        {
            printf(">");
        }
        gets(msg.cmd);
        if (strlen(msg.cmd) == 0)
        {
            if (mark == 1)
            {
                continue;
            }
        }
        mark = 1;
        // 处理命令并向服务器发送消息
        ret = cmd_handler(msg, c_fd);
        if (ret > IFGO)
        {
            putchar('>');
            fflush(stdout);
            continue;
        }
        if (ret == -1)
        {
            printf("cmd not \n");
            printf(">");
            fflush(stdout);
            continue;
        }
        // 读取服务器返回的消息
        serverMsgHandler(msg, c_fd);
    }
    // 4. 关闭socket
    close(c_fd);

    return 0;
}
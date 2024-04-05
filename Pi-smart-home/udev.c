/** 守护进程 */

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

static bool flag = true; // 定义全局标志位

// 信号处理函数，设置标志位为false，用于退出程序
void handler(int sig)
{
    printf("I got a signal %d\nI'm quitting.\n", sig);
    flag = false;
}

// 判断进程是否已经在运行
int judMent()
{
    FILE *file;
    char buffer[128] = {'\0'};
    char *cmd = "ps -elf | grep SmartHome | grep -v grep"; // 查找包含 SmartHome 关键词的进程
    file = popen(cmd, "r");                                // 执行系统命令并打开管道
    fgets(buffer, 128, file);                              // 读取管道内容到 buffer 中
    if (strstr(buffer, "SmartHome") != NULL)               // 如果在 buffer 中找到 SmartHome 关键词
    {
        return 0; // 返回0，表示已经找到进程
    }
    else
    {
        return -1; // 返回-1，表示未找到进程
    }
    printf("BUFFER:%s\n", buffer);
}

int main()
{
    time_t t;
    int fd;

    // 创建守护进程
    if (-1 == daemon(0, 0))
    {
        printf("daemon error\n");
        exit(1);
    }

    // 设置信号处理函数，当收到 SIGQUIT 信号时调用 handler 函数
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (sigaction(SIGQUIT, &act, NULL))
    {
        printf("sigaction error.\n");
        exit(0);
    }

    sleep(30); // 等待30秒，确保 SmartHome 进程完全启动

    // 进程工作内容
    while (flag)
    {
        if (judMent() == -1) // 如果未找到 SmartHome 进程
        {
            system("/home/pi/SmartHome &"); // 执行启动 SmartHome 进程的命令
        }
        sleep(5); // 每隔5秒检查一次进程状态
    }

    return 0;
}

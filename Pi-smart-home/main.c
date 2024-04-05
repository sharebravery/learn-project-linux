#include <stdio.h>
#include "contrlDevices.h" // 包含设备控制相关的头文件
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "InputCommand.h" // 包含输入命令相关的头文件

// 全局变量声明
struct Devices *pdeviceHead = NULL;          // 设备链表头指针
struct InputCommander *pCommandHead = NULL;  // 命令链表头指针
struct InputCommander *socketHandler = NULL; // Socket 命令处理器
struct Devices *deviceTmp = NULL;            // 临时设备指针

int c_fd; // Socket 客户端文件描述符

// 根据设备名称在设备链表中查找设备
struct Devices *findDeviceByName(char *name, struct Devices *phead)
{
    struct Devices *tmp = phead;
    if (phead == NULL)
    {
        return NULL;
    }
    else
    {
        while (tmp != NULL)
        {
            if (strcmp(tmp->deviceName, name) == 0)
            {
                return tmp;
            }
            tmp = tmp->next;
        }
        return NULL;
    }
}

// 根据命令名称在命令链表中查找命令
struct InputCommander *findCommandByName(char *name, struct InputCommander *phead)
{
    struct InputCommander *tmp = phead;
    if (phead == NULL)
    {
        return NULL;
    }
    else
    {
        while (tmp != NULL)
        {
            if (strcmp(tmp->commandName, name) == 0)
            {
                return tmp;
            }
            tmp = tmp->next;
        }
        return NULL;
    }
}

// 开启指定名称的灯
void openLight(char deviceName[32])
{
    deviceTmp = findDeviceByName(deviceName, pdeviceHead);
    deviceTmp->deviceInit(deviceTmp->pinNum);
    deviceTmp->open(deviceTmp->pinNum);
}

// 关闭指定名称的灯
void closeLight(char deviceName[32])
{
    deviceTmp = findDeviceByName(deviceName, pdeviceHead);
    deviceTmp->deviceInit(deviceTmp->pinNum);
    deviceTmp->close(deviceTmp->pinNum);
}

// 同时开启两个楼层的灯
void openGroundFloorLight(char deviceName1[32], char deviceName2[32])
{
    deviceTmp = findDeviceByName(deviceName1, pdeviceHead);
    deviceTmp->deviceInit(deviceTmp->pinNum);
    deviceTmp->open(deviceTmp->pinNum);

    deviceTmp = findDeviceByName(deviceName2, pdeviceHead);
    deviceTmp->deviceInit(deviceTmp->pinNum);
    deviceTmp->open(deviceTmp->pinNum);
}

// 同时关闭两个楼层的灯
void closeGroundFloorLight(char deviceName1[32], char deviceName2[32])
{
    deviceTmp = findDeviceByName(deviceName1, pdeviceHead);
    deviceTmp->deviceInit(deviceTmp->pinNum);
    deviceTmp->close(deviceTmp->pinNum);

    deviceTmp = findDeviceByName(deviceName2, pdeviceHead);
    deviceTmp->deviceInit(deviceTmp->pinNum);
    deviceTmp->close(deviceTmp->pinNum);
}

// 语音线程，监听语音命令并执行相应操作
void *voice_thread(void *datas)
{
    struct InputCommander *voiceHandler;
    int nread;

    voiceHandler = findCommandByName("voice", pCommandHead);

    if (voiceHandler == NULL)
    {
        printf("find voiceHandler error\n");
        pthread_exit(NULL);
    }
    else
    {
        if (voiceHandler->Init(voiceHandler, NULL, NULL) < 0)
        {
            printf("voice init error\n");
            pthread_exit(NULL);
        }
        else
        {
            printf("%s init success\n", voiceHandler->commandName);
        }
        while (1)
        {
            nread = voiceHandler->getCommand(voiceHandler);
            if (nread < 0)
            {
                printf("voice init error\n");
                pthread_exit(NULL);
            }
            else
            {
                printf("do divece contrl:%s\n", voiceHandler->command);

                // 根据语音命令执行相应操作
                // ...
            }
        }
    }
}

// 读取 Socket 消息的线程
void *read_thread(void *datas)
{
    int n_read;
    memset(socketHandler->command, '\0', sizeof(socketHandler->command));
    n_read = read(c_fd, socketHandler->command, sizeof(socketHandler->command));
    if (n_read == -1)
    {
        perror("read");
    }
    else if (n_read > 0)
    {
        printf("\nget: %d,%s\n", n_read, socketHandler->command);
    }
    else
    {
        printf("client quit\n");
    }
}

// Socket 服务器线程，接收客户端连接并创建读取线程
void *socket_thread(void *datas)
{
    pthread_t readthread;
    struct sockaddr_in c_addr;

    memset(&c_addr, 0, sizeof(struct sockaddr_in));
    int clen = sizeof(struct sockaddr_in);

    socketHandler = findCommandByName("socketServer", pCommandHead);

    if (socketHandler == NULL)
    {
        printf("find socketHandler error\n");
        pthread_exit(NULL);
    }
    else
    {
        printf("%s init success\n", socketHandler->commandName);
    }
    socketHandler->Init(socketHandler, NULL, NULL);
    while (1)
    {
        c_fd = accept(socketHandler->sfd, (struct sockaddr *)&c_addr, &clen);
        pthread_create(&readthread, NULL, read_thread, NULL);
    }
}

// 火灾监测线程，检测火灾状态并输出相应信息
void *fireAlarm_thread(void *datas)
{
    int firestatus;

    struct Devices *fireTmp = NULL;
    fireTmp = findDeviceByName("fireIfOrNot", pdeviceHead);
    fireTmp->deviceInit(fireTmp->pinNum);
    while (1)
    {
        firestatus = fireTmp->readStatus(fireTmp->pinNum);
        if (firestatus == 0)
        {
            printf("fire happen\n");
            delay(1000);
        }
        if (firestatus == 1)
        {
            printf("no fire!!!\n");
            delay(1000);
        }
        delay(1000);
    }
}

int main()
{
    char name[128];

    struct Devices *tmp = NULL;

    pthread_t voiceThread;
    pthread_t socketThread;
    pthread_t fireAlarmThread;

    // wiring库初始化
    if (wiringPiSetup() == -1)
    {
        printf("wiringPi setup error!\n");
        return -1;
    }
    // 初始化指令工厂
    pCommandHead = addvoiceContrlLightToDeviceLink(pCommandHead);
    pCommandHead = addsocketContrlLightToDeviceLink(pCommandHead);

    // 初始化设备控制
    pdeviceHead = addBathroomLightToDeviceLink(pdeviceHead);
    pdeviceHead = addUpstairLightToDeviceLink(pdeviceHead);
    pdeviceHead = addLivingLightToDeviceLink(pdeviceHead);
    pdeviceHead = addrestaurantLightToDeviceLink(pdeviceHead);
    pdeviceHead = addPoolLightToDeviceLink(pdeviceHead);
    pdeviceHead = addFireToDeviceLink(pdeviceHead);

    // 创建线程池

    // 语音线程
    pthread_create(&voiceThread, NULL, voice_thread, NULL);

    // Socket 线程
    pthread_create(&socketThread, NULL, socket_thread, NULL);

    // 火灾线程
    pthread_create(&fireAlarmThread, NULL, fireAlarm_thread, NULL);

    pthread_join(voiceThread, NULL);
    pthread_join(socketThread, NULL);
    pthread_join(fireAlarmThread, NULL);

    return 0;
}

/** 人脸识别 */
#include "contrlDevices.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

void postUrl();
size_t readData1(void *ptr, size_t size, size_t nmemb, void *stream);
char *getFace1();
char *getPicFromOCRBase641(char *Filepath);
struct Devices *addcameraToDeviceLink(struct Devices *phead);
int cameraInit(int pinNum);

char ocrRetBuf[1024] = {'\0'};

size_t readData1(void *ptr, size_t size, size_t nmemb, void *stream)
// 回调函数，把从后台的数据拷贝给ocrRetBuf
{
    strncpy(ocrRetBuf, ptr, 1024); // ocrRetBuf接收从OCR后台返回的数据,并打印
    printf("%s\n", ocrRetBuf);
}

char *getFace1()
{
    printf("pai zhao zhong\n");
    // system("raspistill -q 5 -t 1 -o image.jpg");
    system("wget  http://192.168.101.223:8080/?action=snapshot -O ./image.jpg ");
    while (access("./image.jpg", F_OK) != 0)
        ; // 判断是否拍照完毕

    printf("paizhao wanbi\n");

    char *base64BufFaceRec = getPicFromOCRBase641("./image.jpg");
    // system("rm image.jpg");

    return base64BufFaceRec; // 返回刚才拍照的base64
}

char *getPicFromOCRBase641(char *Filepath)
{
    int fd;
    int filelen;
    char cmd[128] = {'\0'};

    sprintf(cmd, "base64 %s > tmpFile", Filepath);
    system(cmd);

    fd = open("./tmpFile", O_RDWR);
    filelen = lseek(fd, 0, SEEK_END); // 计算文件大小
    lseek(fd, 0, SEEK_SET);           // 移动光标到头

    char *bufpic = (char *)malloc(filelen + 2);
    memset(bufpic, '\0', filelen + 2);
    read(fd, bufpic, filelen + 128); // 把内容读到bufpic内

    system("rm -rf tmpFile"); // 删除这个临时文件
    close(fd);

    return bufpic; // 返回bufpic这个地址
}

void postUrl()
{
    CURL *curl;
    CURLcode res;

    char *key = "5xxxsWxxxGfP2pmxxxchSJ";              // 翔云平台购买人脸识别后的key
    char *secret = "396xxxd33xxx487exxxf139axxx6d789"; // 翔云平台购买人脸识别后的secret
    int typeId = 21;
    char *format = "xml";

    char *base64BufPic1 = getFace1();

    char *base64BufPic2 = getPicFromOCRBase641("lit1.jpg");

    int len = strlen(key) + strlen(secret) + strlen(base64BufPic1) + strlen(base64BufPic2) + 128;
    char *postString = (char *)malloc(len);
    memset(postString, '\0', len);
    sprintf(postString, "img1=%s&img2=%s&key=%s&secret=%s&typeId=%d&format=%s", base64BufPic1, base64BufPic2, key, secret, typeId, format); // 根据平台的传参格式编写

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postString);                   // 指定post内容，传入参数
        curl_easy_setopt(curl, CURLOPT_URL, "https://netocr.com/api/faceliu.do"); // 指定url
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, readData1);                 // 回调函数readDate读取返回值
        res = curl_easy_perform(curl);
        printf("OK:%d\n", res);

        if (strstr(ocrRetBuf, "是") != NULL)
        { // 判断翔云后台返回的字符串中有没有“是”
            printf("是同一个人\n");
            // 这里可以开锁，上锁的操作
        }
        else
        {
            printf("不是同一个人\n");
        }
        curl_easy_cleanup(curl);
    }
}

int cameraInit(int pinNum) // 该函数无用
{
    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);
}

struct Devices camera = {

    .deviceName = "camera",
    .deviceInit = cameraInit,
    .pinNum = 0, // 此引脚号无用
    .justDoOnce = postUrl,
    .getFace = getFace1,
    .getPicFromOCRBase64 = getPicFromOCRBase641,
    .readData = readData1

};

struct Devices *addcameraToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &camera;
    }
    else
    {
        camera.next = phead;
        phead = &camera;
    }
}
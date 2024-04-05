#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *fp;
    char buffer[100];

    // 打开文件以读取
    fp = fopen("example.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    // 读取文件内容到缓冲区
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error reading from file");
        fclose(fp);
        return -1;
    }

    printf("Data read from file: %s", buffer);

    // 关闭文件
    fclose(fp);

    return 0;
}

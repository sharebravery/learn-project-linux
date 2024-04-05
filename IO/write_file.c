#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *fp;
    char data[] = "Hello, world!\n";

    // 打开文件以写入
    fp = fopen("example.txt", "w");
    if (fp == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    // 写入数据到文件
    if (fwrite(data, sizeof(char), sizeof(data), fp) != sizeof(data))
    {
        perror("Error writing to file");
        fclose(fp);
        return -1;
    }

    printf("Data written to file successfully\n");

    // 关闭文件
    fclose(fp);

    return 0;
}

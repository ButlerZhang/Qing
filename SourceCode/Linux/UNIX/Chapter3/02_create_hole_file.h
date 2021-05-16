#pragma once
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
 * 函数功能：创建一个具有空洞的文件。
 *          可以使用ls -l file.hole查看文件大小。
*/
void create_hole_file(void) {
    char buf1[] = "abcdefghij";
    char buf2[] = "ABCDEFGHIJ";
    const int bufSize = 10;

    //首先创建一个文件
    int fd;
    if ((fd = creat("file.hole", FILE_MODE)) < 0) {
        printf("create error\n");
        return;
    }

    //先将一部分内容写入这个文件
    if (write(fd, buf1, bufSize) != bufSize) {
        printf("buf1 write error\n");
        return;
    }

    //文件偏移量比当前文件长度还要大
    if (lseek(fd, 16384, SEEK_SET) == -1) {
        printf("lseek error\n");
        return;
    }

    //把另一部分内容写入文件
    if (write(fd, buf2, bufSize) != bufSize) {
        printf("buf2 write error\n");
        return;
    }
}

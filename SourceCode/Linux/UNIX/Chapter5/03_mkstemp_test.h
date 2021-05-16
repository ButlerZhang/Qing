#pragma once
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数功能：mkstemp函数的应用
 *
 * 测试步骤：
 * ./a.out
*/

void make_temp(char *strtemplate) {
    //mksteamp创建一个唯一名字的临时文件
    int fd;
    if ((fd = mkstemp(strtemplate)) < 0) {
        printf("can not create temp file\n");
        return;
    }

    printf("temp file name = %s\n", strtemplate);

    //手动关闭它，但没有释放，仍被进程占用
    close(fd);

    struct stat sbuf;
    if (stat(strtemplate, &sbuf) < 0) {
        if (errno == ENOENT) {
            printf("file does not exist\n");
        }
        else {
            printf("stat failed\n");
            return;
        }
    }
    else {
        printf("file exists\n");

        //手动unlink，程序结束后自动释放
        unlink(strtemplate);
    }
}

void mkstemp_test(void) {
    //因为要对XXXXXX进行替换，因此应该使用栈上分配的数组
    char good_template[] = "/tmp/dirXXXXXX"; //right way
    char *bad_template = "/tmp/dirXXXXXX";   //wrong way, segment fault

    printf("trying to create first temp file...\n");
    make_temp(good_template);

    printf("trying to create second temp file...\n");
    make_temp(bad_template);
}

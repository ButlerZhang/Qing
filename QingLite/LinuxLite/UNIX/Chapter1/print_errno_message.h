#pragma once
#include <stdio.h>
#include <errno.h>
#include <string.h> //strerror



void print_errno_message(int argc, char *argv[])
{
    //strerror返回指向消息字符串的指针
    fprintf(stderr, "EACCES: %s\n", strerror(EACCES));

    //为了测试主动修改errno的值
    errno = ENOENT;

    //在标准错误上产生一条出错信息
    //格式：先打印msg，然后打印冒号和空格，接着打印errno值的字符串错误信息，最后是换行符。
    perror(argv[0]);
}

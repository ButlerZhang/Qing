#pragma once
#include <stdio.h>
#include <errno.h>
#include <string.h> //strerror

/*
* errno两个常用的函数strerror，perror
*/
void print_errno_message(int argc, char *argv[]) {
    //strerror返回指向消息字符串的指针
    //也就是说，strerror将EACCES解释为对应的字符串
    fprintf(stderr, "EACCES: %s\n", strerror(EACCES));

    //为了测试主动修改errno的值
    errno = ENOENT;

    //在标准错误上产生一条出错信息
    //格式：先打印msg，然后打印冒号和空格，接着打印errno值的字符串错误信息，最后是换行符。
    perror(argv[0]);

    //将argv[0]作为参数传递给perror是标准的UNIX惯例，
    //使用这种方法，在程序作为管道的一部分执行时，可以分清是哪个程序产生了错误信息。
}

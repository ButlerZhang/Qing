#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数功能：对于指定的描述符打印文件标志。

 * 输入示例：
 * (1) ./a.out 0 < /dev/tty
 * (2) ./a.out 1 > temp.foo
 *     cat temp.foo
 * (3) ./a.out 2 2>>temp.foo
 * (4) ./a.out 5 5<>temp.foo #在文件描述符5上打开文件temp.foo以供读写
*/
int print_file_flag(int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage: a.out <descriptor#>\n");
        return -1;
    }

    //F_GETFL：对应于fd的文件状态标志作为函数值返回。
    int val;
    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0) {
        printf("fcntl error for fd = %d\n", atoi(argv[1]));
        return -2;
    }

    //读写属性
    switch (val & O_ACCMODE) {
        case O_RDONLY:
            printf("read only");
            break;

        case O_WRONLY:
            printf("write only");
            break;

        case O_RDWR:
            printf("read write");
            break;

        default:
            printf("unknown access mode\n");
            return -3;
    }

    //追加写
    if (val & O_APPEND) {
        printf(", append");
    }

    //非阻塞模式
    if (val & O_NONBLOCK) {
        printf(", nonblocking");
    }

    //等待写完成（数据和属性）
    if (val & O_SYNC) {
        printf(", synchronous writes");
    }

    //等待写完成（仅FreeBSD和Mac OS X)
#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC != O_SYNC)
    if (val & O_FSYNC) {
        printf(", synchronous wirtes");
    }
#endif

    putchar('\n');
    return 0;
}

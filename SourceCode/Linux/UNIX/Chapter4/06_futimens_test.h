#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

/*
 * 函数功能：修改文件内容，但不修改访问时间和修改时间。
 *
 * 测试步骤：
 * ls -l  changemod times      //查看长度和最后修改时间
 * ls -lu changemod times      //查看最后访问时间
 * date                        //打印当天日期
 * ./a.out changemod times     //运行程序
 * ls -l  changemod times      //检查结果
 * ls -lu changemod times      //检查最后访问时间
 * ls -lc changemod times      //检查状态更改时间
*/
void futimens_test(int argc, char *argv[]) {
    int fd;
    struct stat statbuf;
    struct timespec times[2];

    for (int i = 1; i < argc; i++) {
        //获取当前时间
        if (stat(argv[i], &statbuf) < 0) {
            printf("%s: stat error", argv[i]);
            continue;
        }

        //截断文件长度为0
        if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) {
            printf("%s: open error", argv[i]);
            continue;
        }

        //不去更改文件的访问时间和修改时间
        times[0] = statbuf.st_atim;
        times[1] = statbuf.st_mtim;

        //重置时间
        if (futimens(fd, times) < 0) {
            printf("%s: futimens error", argv[i]);
        }

        close(fd);
    }
}

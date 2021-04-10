#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

/*
 * 函数功能：测试chmod函数。
 *
 * 测试步骤：
 * 依赖于03_umask_test创建的文件。
*/
void chmod_test(void) {
    struct stat statbuf;

    //获取文件属性
    if (stat("foo", &statbuf) < 0) {
        printf("stat error for foo\n");
        return;
    }

    //turn on set-group-ID and turn off group-execute
    if (chmod("foo", (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0) {
        printf("chmod error for foo\n");
    }

    //set absolute mode to "rw-r--r--"
    if (chmod("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0) {
        printf("chmod error for bar\n");
    }
}

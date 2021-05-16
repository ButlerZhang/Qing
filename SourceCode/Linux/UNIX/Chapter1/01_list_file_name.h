#pragma once
#include <stdio.h>
#include <errno.h>
#include <dirent.h>

/*
 * 函数功能：列出一个目录中所有文件的名字。
 *          可以认为是ls命令的简单实现。
*/
void list_file_name(int argc, char *argv[]) {
    DIR *dp;
    struct dirent *dirp;

    if (argc != 2) {
        printf("usage: ls directory_name\n");
        return;
    }

    if ((dp = opendir(argv[1])) == NULL) {
        printf("can not open %s\n", argv[1]);
        printf("errno = %d\n", errno);
        return;
    }

    while ((dirp = readdir(dp)) != NULL) {
        printf("%s\n", dirp->d_name);
    }

    closedir(dp);
}

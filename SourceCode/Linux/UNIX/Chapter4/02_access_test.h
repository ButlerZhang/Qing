#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/*
 * 函数功能：测试access函数。
 *
 * 测试步骤：
 * ls -l a.out
 * ./a.out a.out
 * ls -l /etc/shadow
 * ./a.out /etc/shadow
 * su
 * chown root a.out #将文件用户ID改为root
 * chmod u+s a.out  #并打开设置用户ID位
 * ls -l a.out
 * exit
 * ./a.out /etc/shadow
*/
void access_test(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: a.out <pathname>\n");
        return;
    }

    //R_OK测试读权限，W_OK测试写权限，X_OK测试执行权限
    if (access(argv[1], R_OK) < 0) {
        printf("access error for %s: %s\n", argv[1], strerror(errno));
    }
    else {
        printf("read access OK\n");
    }

    //以只读方式打开
    if (open(argv[1], O_RDONLY) < 0) {
        printf("open error for %s: %s\n", argv[1], strerror(errno));
    }
    else {
        printf("open for reading OK\n");
    }
}

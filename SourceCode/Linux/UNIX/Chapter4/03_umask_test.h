#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

/*
 * 函数功能：测试umask函数。
 *
 * 测试步骤：
 * umask #先打印当前文件模式创建屏蔽字
 * ./a.out
 * ls -l foo bar
 * umask #观察文件模式创建屏蔽字是否更改
*/
void umask_test(void) {

#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

    //umask函数为进程设置文件模式创建屏蔽字，并返回之前的值
    //在进程创建一个新文件或目录时，一定会使用文件模式创建屏蔽字，屏蔽部分权限
    umask(0);
    if (creat("foo", RWRWRW) < 0) {
        printf("create error for foo\n");
        return;
    }

    //在这里，umask禁止所有组和其它用户的访问权限
    //在文件模式创建屏蔽字中为1的位，在文件mode中的相应位一定被关闭
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (creat("bar", RWRWRW) < 0) {
        printf("create error for bar");
    }
}

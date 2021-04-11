#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/*
 * 函数功能：打开一个文件，然后unlink它。
 *
 * 测试步骤：
 * 依赖于03_umask_test创建的文件。
 *
 * ls -l foo    #查看文件大小
 * ./a.out &    #运行程序
 * ls -l foo    #观察文件是否仍然存在
*/
void unlink_test(void) {
    const char *pathName = "foo";

    if (open(pathName, O_RDWR) < 0) {
        printf("open error\n");
        return;
    }

    //删除现有的目录项，失败返回-1，成功返回0
    if (unlink(pathName) < 0) {
        printf("unlink error\n");
        return;
    }

    printf("file unlinked\n");

    //睡眠是为了演示进程仍然打开着这个文件
    sleep(30);

    printf("done\n");
}

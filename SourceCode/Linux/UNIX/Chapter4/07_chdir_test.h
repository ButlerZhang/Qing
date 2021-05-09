#pragma once
#include <stdio.h>
#include <unistd.h>

/*
 * 函数功能：修改当前工作目录
 *
 * 测试步骤：
 * ./a.out
 * ls -l /home/Butler/test
*/
void chdir_test(void) {
    //此目录需要存在
    const char *path = "/home/butler/Butler";

    //修改当前工作目录
    if (chdir(path) < 0) {
        printf("chdir failed.\n");
        return;
    }

    //获取新的工作目录
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        printf("getcwd failed.\n");
        return;
    }

    //输出判断是否正确
    printf("cwd = %s\n", buffer);
}

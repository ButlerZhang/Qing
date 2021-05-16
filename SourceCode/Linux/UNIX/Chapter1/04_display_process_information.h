#pragma once
#include <stdio.h>
#include <unistd.h>

/*
* 函数功能：打印进程信息。
*/
void display_process_information(void) {
    printf("file name = %s\n", __FILE__);
    printf("current line = %d\n", __LINE__);

    //getpid返回的是pid_t数据类型，我们不知道它的大小。
    //虽然进程ID可以用整型表示，但是用long可以提高可移植性。
    printf("process id = %ld\n", (long)getpid());
    printf("user id = %ld\n", (long)getuid());
    printf("group id = %ld\n", (long)getgid());
}

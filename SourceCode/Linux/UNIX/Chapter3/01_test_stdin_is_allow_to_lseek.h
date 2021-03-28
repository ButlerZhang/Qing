#pragma once
#include <stdio.h>
#include <unistd.h>

/*
* 测试标准输入能否设文件置偏移量。
*/
void test_stdin_is_allow_lseek(void) {
    //可以使用如下方法获取当前文件的偏移量
    //off_t currpos = lseek(fd, 0, SEEK_CUR);

    //也可以用来测试文件是否可以设置偏移量，例如管
    //道、FIFO和Socket，lseek返回-1，表示不能设置，
    //并将errno设置为ESPIPE。

    //这里以标准输入为例子进行测试
    if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1) {
        printf("can not seek\n");
    }
    else {
        printf("seek ok\n");
    }
}

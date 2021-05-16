#pragma once
#include <fcntl.h>
#include <stdio.h>

/*
 * 函数功能：设置一个或多个文件状态标志。
 *
 * 在修改文件描述符标志或文件状态标志时必须谨慎，先要获得
 * 现在的标志值，然后按照期望值修改它，最后设置新标志值。不
 * 能只是执行F_SETFD或F_SETFL命令，这样会关闭以前设置的
 * 标志位。
 *
 * 把中间的语句改为：val &= ~flags; //turn flags off
 * 就构成了另一个函数clr_fl。
*/
bool set_file_flags(int fd, int flags) {

    //先获取旧值
    int val;
    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        printf("fcntl F_GETFL error\n");
        return false;
    }

    //turn on flags
    val |= flags;

    //设置新值
    if (fcntl(fd, F_SETFL, val) < 0) {
        printf("fcntl F_SETFL error\n");
        return false;

    }

    return true;
}

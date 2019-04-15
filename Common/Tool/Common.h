#pragma once
#include <fcntl.h>



bool IsBigEndian()
{
    union test_union
    {
        short m_value;
        char  union_bytes[sizeof(short)];
    };

    test_union test;
    test.m_value = 0x0102;

    return (test.union_bytes[0] == 1 && test.union_bytes[2] == 2);
}



int SetNonBlocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);            //获取文件描述符旧的状态标志
    int new_option = old_option | O_NONBLOCK;       //设置非阻塞状态
    fcntl(fd, F_SETFL, new_option);                 //设置新的状态
    return old_option;                              //返回旧的状态标志，以便后续恢复使用
}

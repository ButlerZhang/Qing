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
    int old_option = fcntl(fd, F_GETFL);            //��ȡ�ļ��������ɵ�״̬��־
    int new_option = old_option | O_NONBLOCK;       //���÷�����״̬
    fcntl(fd, F_SETFL, new_option);                 //�����µ�״̬
    return old_option;                              //���ؾɵ�״̬��־���Ա�����ָ�ʹ��
}

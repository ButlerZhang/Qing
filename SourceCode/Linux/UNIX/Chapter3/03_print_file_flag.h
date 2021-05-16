#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * �������ܣ�����ָ������������ӡ�ļ���־��

 * ����ʾ����
 * (1) ./a.out 0 < /dev/tty
 * (2) ./a.out 1 > temp.foo
 *     cat temp.foo
 * (3) ./a.out 2 2>>temp.foo
 * (4) ./a.out 5 5<>temp.foo #���ļ�������5�ϴ��ļ�temp.foo�Թ���д
*/
int print_file_flag(int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage: a.out <descriptor#>\n");
        return -1;
    }

    //F_GETFL����Ӧ��fd���ļ�״̬��־��Ϊ����ֵ���ء�
    int val;
    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0) {
        printf("fcntl error for fd = %d\n", atoi(argv[1]));
        return -2;
    }

    //��д����
    switch (val & O_ACCMODE) {
        case O_RDONLY:
            printf("read only");
            break;

        case O_WRONLY:
            printf("write only");
            break;

        case O_RDWR:
            printf("read write");
            break;

        default:
            printf("unknown access mode\n");
            return -3;
    }

    //׷��д
    if (val & O_APPEND) {
        printf(", append");
    }

    //������ģʽ
    if (val & O_NONBLOCK) {
        printf(", nonblocking");
    }

    //�ȴ�д��ɣ����ݺ����ԣ�
    if (val & O_SYNC) {
        printf(", synchronous writes");
    }

    //�ȴ�д��ɣ���FreeBSD��Mac OS X)
#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC != O_SYNC)
    if (val & O_FSYNC) {
        printf(", synchronous wirtes");
    }
#endif

    putchar('\n');
    return 0;
}

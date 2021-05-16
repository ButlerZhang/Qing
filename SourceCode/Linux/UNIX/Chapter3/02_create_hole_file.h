#pragma once
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
 * �������ܣ�����һ�����пն����ļ���
 *          ����ʹ��ls -l file.hole�鿴�ļ���С��
*/
void create_hole_file(void) {
    char buf1[] = "abcdefghij";
    char buf2[] = "ABCDEFGHIJ";
    const int bufSize = 10;

    //���ȴ���һ���ļ�
    int fd;
    if ((fd = creat("file.hole", FILE_MODE)) < 0) {
        printf("create error\n");
        return;
    }

    //�Ƚ�һ��������д������ļ�
    if (write(fd, buf1, bufSize) != bufSize) {
        printf("buf1 write error\n");
        return;
    }

    //�ļ�ƫ�����ȵ�ǰ�ļ����Ȼ�Ҫ��
    if (lseek(fd, 16384, SEEK_SET) == -1) {
        printf("lseek error\n");
        return;
    }

    //����һ��������д���ļ�
    if (write(fd, buf2, bufSize) != bufSize) {
        printf("buf2 write error\n");
        return;
    }
}

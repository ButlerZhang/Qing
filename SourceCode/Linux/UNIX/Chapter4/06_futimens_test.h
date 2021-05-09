#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

/*
 * �������ܣ��޸��ļ����ݣ������޸ķ���ʱ����޸�ʱ�䡣
 *
 * ���Բ��裺
 * ls -l  changemod times      //�鿴���Ⱥ�����޸�ʱ��
 * ls -lu changemod times      //�鿴������ʱ��
 * date                        //��ӡ��������
 * ./a.out changemod times     //���г���
 * ls -l  changemod times      //�����
 * ls -lu changemod times      //���������ʱ��
 * ls -lc changemod times      //���״̬����ʱ��
*/
void futimens_test(int argc, char *argv[]) {
    int fd;
    struct stat statbuf;
    struct timespec times[2];

    for (int i = 1; i < argc; i++) {
        //��ȡ��ǰʱ��
        if (stat(argv[i], &statbuf) < 0) {
            printf("%s: stat error", argv[i]);
            continue;
        }

        //�ض��ļ�����Ϊ0
        if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) {
            printf("%s: open error", argv[i]);
            continue;
        }

        //��ȥ�����ļ��ķ���ʱ����޸�ʱ��
        times[0] = statbuf.st_atim;
        times[1] = statbuf.st_mtim;

        //����ʱ��
        if (futimens(fd, times) < 0) {
            printf("%s: futimens error", argv[i]);
        }

        close(fd);
    }
}

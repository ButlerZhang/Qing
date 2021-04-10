#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

/*
 * �������ܣ�����chmod������
 *
 * ���Բ��裺
 * ������03_umask_test�������ļ���
*/
void chmod_test(void) {
    struct stat statbuf;

    //��ȡ�ļ�����
    if (stat("foo", &statbuf) < 0) {
        printf("stat error for foo\n");
        return;
    }

    //turn on set-group-ID and turn off group-execute
    if (chmod("foo", (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0) {
        printf("chmod error for foo\n");
    }

    //set absolute mode to "rw-r--r--"
    if (chmod("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0) {
        printf("chmod error for bar\n");
    }
}

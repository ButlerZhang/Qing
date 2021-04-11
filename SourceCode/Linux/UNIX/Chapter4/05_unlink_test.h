#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/*
 * �������ܣ���һ���ļ���Ȼ��unlink����
 *
 * ���Բ��裺
 * ������03_umask_test�������ļ���
 *
 * ls -l foo    #�鿴�ļ���С
 * ./a.out &    #���г���
 * ls -l foo    #�۲��ļ��Ƿ���Ȼ����
*/
void unlink_test(void) {
    const char *pathName = "foo";

    if (open(pathName, O_RDWR) < 0) {
        printf("open error\n");
        return;
    }

    //ɾ�����е�Ŀ¼�ʧ�ܷ���-1���ɹ�����0
    if (unlink(pathName) < 0) {
        printf("unlink error\n");
        return;
    }

    printf("file unlinked\n");

    //˯����Ϊ����ʾ������Ȼ��������ļ�
    sleep(30);

    printf("done\n");
}

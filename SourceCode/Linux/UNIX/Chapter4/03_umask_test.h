#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

/*
 * �������ܣ�����umask������
 *
 * ���Բ��裺
 * umask #�ȴ�ӡ��ǰ�ļ�ģʽ����������
 * ./a.out
 * ls -l foo bar
 * umask #�۲��ļ�ģʽ�����������Ƿ����
*/
void umask_test(void) {

#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

    //umask����Ϊ���������ļ�ģʽ���������֣�������֮ǰ��ֵ
    //�ڽ��̴���һ�����ļ���Ŀ¼ʱ��һ����ʹ���ļ�ģʽ���������֣����β���Ȩ��
    umask(0);
    if (creat("foo", RWRWRW) < 0) {
        printf("create error for foo\n");
        return;
    }

    //�����umask��ֹ������������û��ķ���Ȩ��
    //���ļ�ģʽ������������Ϊ1��λ�����ļ�mode�е���Ӧλһ�����ر�
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (creat("bar", RWRWRW) < 0) {
        printf("create error for bar");
    }
}

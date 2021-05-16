#pragma once
#include <stdio.h>
#include <stdlib.h>

/*
 * �������ܣ�mkstemp������Ӧ��
 *
 * ���Բ��裺
 * ./a.out
*/

void make_temp(char *strtemplate) {
    //mksteamp����һ��Ψһ���ֵ���ʱ�ļ�
    int fd;
    if ((fd = mkstemp(strtemplate)) < 0) {
        printf("can not create temp file\n");
        return;
    }

    printf("temp file name = %s\n", strtemplate);

    //�ֶ��ر�������û���ͷţ��Ա�����ռ��
    close(fd);

    struct stat sbuf;
    if (stat(strtemplate, &sbuf) < 0) {
        if (errno == ENOENT) {
            printf("file does not exist\n");
        }
        else {
            printf("stat failed\n");
            return;
        }
    }
    else {
        printf("file exists\n");

        //�ֶ�unlink������������Զ��ͷ�
        unlink(strtemplate);
    }
}

void mkstemp_test(void) {
    //��ΪҪ��XXXXXX�����滻�����Ӧ��ʹ��ջ�Ϸ��������
    char good_template[] = "/tmp/dirXXXXXX"; //right way
    char *bad_template = "/tmp/dirXXXXXX";   //wrong way, segment fault

    printf("trying to create first temp file...\n");
    make_temp(good_template);

    printf("trying to create second temp file...\n");
    make_temp(bad_template);
}

#pragma once
#include <stdio.h>
#include <errno.h>
#include <string.h> //strerror



void print_errno_message(int argc, char *argv[])
{
    //strerror����ָ����Ϣ�ַ�����ָ��
    fprintf(stderr, "EACCES: %s\n", strerror(EACCES));

    //Ϊ�˲��������޸�errno��ֵ
    errno = ENOENT;

    //�ڱ�׼�����ϲ���һ��������Ϣ
    //��ʽ���ȴ�ӡmsg��Ȼ���ӡð�źͿո񣬽��Ŵ�ӡerrnoֵ���ַ���������Ϣ������ǻ��з���
    perror(argv[0]);
}

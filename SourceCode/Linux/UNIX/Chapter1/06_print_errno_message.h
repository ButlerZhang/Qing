#pragma once
#include <stdio.h>
#include <errno.h>
#include <string.h> //strerror

/*
* errno�������õĺ���strerror��perror
*/
void print_errno_message(int argc, char *argv[]) {
    //strerror����ָ����Ϣ�ַ�����ָ��
    //Ҳ����˵��strerror��EACCES����Ϊ��Ӧ���ַ���
    fprintf(stderr, "EACCES: %s\n", strerror(EACCES));

    //Ϊ�˲��������޸�errno��ֵ
    errno = ENOENT;

    //�ڱ�׼�����ϲ���һ��������Ϣ
    //��ʽ���ȴ�ӡmsg��Ȼ���ӡð�źͿո񣬽��Ŵ�ӡerrnoֵ���ַ���������Ϣ������ǻ��з���
    perror(argv[0]);

    //��argv[0]��Ϊ�������ݸ�perror�Ǳ�׼��UNIX������
    //ʹ�����ַ������ڳ�����Ϊ�ܵ���һ����ִ��ʱ�����Է������ĸ���������˴�����Ϣ��
}

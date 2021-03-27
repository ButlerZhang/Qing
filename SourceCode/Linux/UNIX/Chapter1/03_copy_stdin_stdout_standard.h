#pragma once
#include <stdio.h>

/*
 * ��׼IO�����Դ������IO��
*/

void copy_stdin_stdout_standard(void) {
    int c;

    //getcÿ�ζ�ȡһ���ַ���putc������ַ�д����׼�����
    //������������һ���ֽ�ʱ��getc���س���EOF����ʾ������
    //stdin��ʾ��׼���룬stdout��ʾ��׼��������Ǹ��ļ���
    //����������ͬ�Ķ���ֻ������ͬ�ĺ��塣
    while ((c = getc(stdin)) != EOF) {
        if (putc(c, stdout) == EOF) {
            printf("output error");
        }
    }

    if (ferror(stdin)) {
        printf("input error");
    }
}

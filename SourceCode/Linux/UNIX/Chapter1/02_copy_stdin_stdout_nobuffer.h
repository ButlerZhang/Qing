#pragma once
#include <unistd.h>

#define CHAPTER1_BUFFSIZE 4096

/*
 * �������ܣ����������IO���ӱ�׼����������׼���д��
 * 
 *          ���������IO��������Ҫ����Ա�Զ�����ʵ�
 *          ������������ĺ�����Ҫ�������Բ��ܵ�֪��
*/
void copy_stdin_stdout_nobuffer(void) {
    long int     n;
    char         buf[CHAPTER1_BUFFSIZE];

    //STDIN_FILENO��ʾ��׼���룬STDOUT_FILENO��ʾ��׼�����
    //��Ȼ������0��1�ֱ��ʾ�����ɶ��Ժ���ֲ�Զ����á�
    while ((n = read(STDIN_FILENO, buf, CHAPTER1_BUFFSIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) {
            printf("write error = %d\n", errno);
            return;
        }
    }

    if (n < 0) {
        printf("read error");
    }
}

#pragma once
#include <stdio.h>

/*
 * �������ܣ�tmpnam��tmpfile����ʵ��
 *
 * ���Բ��裺
 * ./a.out
*/

void temporary_file(void) {
    //tmpnam��������һ���������ļ�����ͬ��һ����Ч·�����ַ�����
    //ÿ�ε�����ʱ��������һ����ͬ��·������������TMP_MAX�Σ�
    //������NULL����������·���������һ����̬���У����ظþ�̬����ָ�룻
    //�����ٵ���tmpnamʱ������д�þ�̬���������Ҫ�Լ����淵��ֵ��
    //�������NULL��������ǳ�������L_tmpnam���ַ����顣
    printf("first  temp name = %s\n", tmpnam(NULL));

    char name[L_tmpnam];
    tmpnam(name);
    printf("second temp name = %s\n", name);

    //tmpfile����һ����ʱ�������ļ�(wb+)���ڹرո��ļ������
    //����ʱ���Զ�ɾ�������ļ���UNIX�Զ������ļ��������������֡�
    FILE *fp;
    if ((fp = tmpfile()) == NULL) {
        printf("tmpfile error\n");
        return;
    }

    //д��һ���ַ�������ʱ�ļ�
    fputs("one line of output\n", fp);

    //���½������õ���ʼλ��
    rewind(fp);

    //�Ѹղ�д����ַ������¶�ȡ����
    char line[1024];
    if (fgets(line, sizeof(line), fp) == NULL) {
        printf("fgets error\n");
        return;
    }

    //�������׼���
    fputs(line, stdout);
}

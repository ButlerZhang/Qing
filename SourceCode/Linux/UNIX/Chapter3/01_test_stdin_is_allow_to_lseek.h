#pragma once
#include <stdio.h>
#include <unistd.h>

/*
* ���Ա�׼�����ܷ����ļ���ƫ������
*/
void test_stdin_is_allow_lseek(void) {
    //����ʹ�����·�����ȡ��ǰ�ļ���ƫ����
    //off_t currpos = lseek(fd, 0, SEEK_CUR);

    //Ҳ�������������ļ��Ƿ��������ƫ�����������
    //����FIFO��Socket��lseek����-1����ʾ�������ã�
    //����errno����ΪESPIPE��

    //�����Ա�׼����Ϊ���ӽ��в���
    if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1) {
        printf("can not seek\n");
    }
    else {
        printf("seek ok\n");
    }
}

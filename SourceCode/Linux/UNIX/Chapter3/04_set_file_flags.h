#pragma once
#include <fcntl.h>
#include <stdio.h>

/*
 * �������ܣ�����һ�������ļ�״̬��־��
 *
 * ���޸��ļ���������־���ļ�״̬��־ʱ�����������Ҫ���
 * ���ڵı�־ֵ��Ȼ��������ֵ�޸�������������±�־ֵ����
 * ��ֻ��ִ��F_SETFD��F_SETFL���������ر���ǰ���õ�
 * ��־λ��
 *
 * ���м������Ϊ��val &= ~flags; //turn flags off
 * �͹�������һ������clr_fl��
*/
bool set_file_flags(int fd, int flags) {

    //�Ȼ�ȡ��ֵ
    int val;
    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        printf("fcntl F_GETFL error\n");
        return false;
    }

    //turn on flags
    val |= flags;

    //������ֵ
    if (fcntl(fd, F_SETFL, val) < 0) {
        printf("fcntl F_SETFL error\n");
        return false;

    }

    return true;
}

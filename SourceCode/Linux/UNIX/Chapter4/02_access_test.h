#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/*
 * �������ܣ�����access������
 *
 * ���Բ��裺
 * ls -l a.out
 * ./a.out a.out
 * ls -l /etc/shadow
 * ./a.out /etc/shadow
 * su
 * chown root a.out #���ļ��û�ID��Ϊroot
 * chmod u+s a.out  #���������û�IDλ
 * ls -l a.out
 * exit
 * ./a.out /etc/shadow
*/
void access_test(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: a.out <pathname>\n");
        return;
    }

    //R_OK���Զ�Ȩ�ޣ�W_OK����дȨ�ޣ�X_OK����ִ��Ȩ��
    if (access(argv[1], R_OK) < 0) {
        printf("access error for %s: %s\n", argv[1], strerror(errno));
    }
    else {
        printf("read access OK\n");
    }

    //��ֻ����ʽ��
    if (open(argv[1], O_RDONLY) < 0) {
        printf("open error for %s: %s\n", argv[1], strerror(errno));
    }
    else {
        printf("open for reading OK\n");
    }
}

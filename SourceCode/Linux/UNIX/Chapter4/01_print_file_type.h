#pragma once
#include <stdio.h>
#include <sys/stat.h>

/*
 * ���������в�������ӡ�ļ����͡�
 *
 * ������������������ԣ�
 * ./a.out /etc/passwd /etc /dev/log /dev/tty \
 * /var/lib/oprofile/opd_pipe /dev/sr0 /dev/cdrom
*/
void print_file_type(int argc, char *argv[]) {

    //�����ļ�������Ϣ�Ľṹ
    struct stat buf;

    const char *ptr;
    for (int i = 1; i < argc; i++) {
        printf("%s: ", argv[i]);

        //lstat�������stat���������㣬��Ϊ��
        //���Է��ط������ӵ��ļ���Ϣ�ṹ��
        if (lstat(argv[i], &buf) < 0) {
            printf("lstat error\n");
            continue;
        }

        //����ʹ������ĸ�����ȷ���ļ�����
        if (S_ISREG(buf.st_mode)) {
            ptr = "regular";
        }
        else if (S_ISDIR(buf.st_mode)) {
            ptr = "directory";
        }
        else if (S_ISCHR(buf.st_mode)) {
            ptr = "character special";
        }
        else if (S_ISBLK(buf.st_mode)) {
            ptr = "block special";
        }
        else if (S_ISFIFO(buf.st_mode)) {
            ptr = "FIFO";
        }
        else if (S_ISLNK(buf.st_mode)) {
            ptr = "symbolic link";
        }
        else if (S_ISSOCK(buf.st_mode)) {
            ptr = "socket";
        }
        else {
            ptr = "** unknown mode **";
        }

        printf("%s\n", ptr);
    }
}

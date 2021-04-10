#pragma once
#include <stdio.h>
#include <sys/stat.h>

/*
 * 根据命令行参数，打印文件类型。
 *
 * 可以输入如下命令测试：
 * ./a.out /etc/passwd /etc /dev/log /dev/tty \
 * /var/lib/oprofile/opd_pipe /dev/sr0 /dev/cdrom
*/
void print_file_type(int argc, char *argv[]) {

    //保存文件各种信息的结构
    struct stat buf;

    const char *ptr;
    for (int i = 1; i < argc; i++) {
        printf("%s: ", argv[i]);

        //lstat函数会比stat函数更方便，因为它
        //可以返回符号链接的文件信息结构。
        if (lstat(argv[i], &buf) < 0) {
            printf("lstat error\n");
            continue;
        }

        //可以使用下面的各个宏确定文件类型
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

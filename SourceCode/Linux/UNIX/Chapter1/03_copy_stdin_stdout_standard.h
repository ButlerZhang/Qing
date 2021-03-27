#pragma once
#include <stdio.h>

/*
 * 标准IO，即自带缓冲的IO。
*/

void copy_stdin_stdout_standard(void) {
    int c;

    //getc每次读取一个字符，putc将这个字符写到标准输出。
    //读到输入的最后一个字节时，getc返回常量EOF，表示结束。
    //stdin表示标准输入，stdout表示标准输出。它们跟文件描
    //述符不是相同的对象，只是有相同的含义。
    while ((c = getc(stdin)) != EOF) {
        if (putc(c, stdout) == EOF) {
            printf("output error");
        }
    }

    if (ferror(stdin)) {
        printf("input error");
    }
}

#pragma once
#include <unistd.h>

#define CHAPTER1_BUFFSIZE 4096

/*
 * 函数功能：不带缓冲的IO，从标准输入读，向标准输出写。
 * 
 *          不带缓冲的IO函数，需要程序员自定义合适的
 *          缓冲区，这里的合适需要经过测试才能得知。
*/
void copy_stdin_stdout_nobuffer(void) {
    long int     n;
    char         buf[CHAPTER1_BUFFSIZE];

    //STDIN_FILENO表示标准输入，STDOUT_FILENO表示标准输出，
    //虽然可以用0和1分别表示，但可读性和移植性都不好。
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

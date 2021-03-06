#pragma once
#include <unistd.h>



void copy_stdin_stdout_nobuffer(void)
{
#define BUFFSIZE 4096

    long int     n;
    char         buf[BUFFSIZE];

    while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) {
            printf("write error");
        }
    }

    if (n < 0) {
        printf("read error");
    }
}

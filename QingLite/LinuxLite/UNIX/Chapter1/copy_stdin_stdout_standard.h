#pragma once
#include <stdio.h>



void copy_stdin_stdout_standard(void)
{
    int c;
    while ((c = getc(stdin)) != EOF) {
        if (putc(c, stdout) == EOF) {
            printf("output error");
        }
    }

    if (ferror(stdin)) {
        printf("input error");
    }
}
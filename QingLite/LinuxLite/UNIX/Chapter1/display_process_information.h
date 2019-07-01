#pragma once
#include <stdio.h>
#include <unistd.h>



void display_process_information(void)
{
    printf("file name = %s\n", __FILE__);
    printf("current line = %d\n", __LINE__);

    printf("process id = %ld\n", (long)getpid());
    printf("user id = %ld\n", (long)getuid());
    printf("group id = %ld\n", (long)getgid());
}

#pragma once
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>



void exec_command_like_shell(void)
{
    const int MAXLINE = 1024;
    char command_buffer[MAXLINE];

    printf("%% ");

    //标准IO函数fgets从标准输入一次读取一行
    while (fgets(command_buffer, MAXLINE, stdin) != NULL) {

        //使用null字节替换换行符，用于execlp函数
        if (command_buffer[strlen(command_buffer) - 1] == '\n') {
            command_buffer[strlen(command_buffer) - 1] = 0;
        }

        //fork创建一个新进程，返回两次
        pid_t pid;
        if ((pid = fork()) < 0) {
            printf("fork error\n");
            continue;
        }

        //child
        //fork和execlp函数的组合，即是常说的产生(spawn)一个新进程
        if (pid == 0) {
            execlp(command_buffer, command_buffer, (char*)0);
            printf("could not execute: %s\n", command_buffer);
            exit(127);
        }

        //parent
        //父进程希望等待子进程终止，status可以判断子进程是如何终止的
        int status = 0;
        if ((pid = waitpid(pid, &status, 0)) < 0) {
            printf("waitpid error\n");
        }

        printf("%% ");
    }

    exit(0);
}

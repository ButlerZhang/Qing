#pragma once
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
* 函数功能：从标准输入读取命令，然后执行这些命令。
*          这个程序的最主要限制是不能向所执行的命令
*          传递参数，但是足够用来说明UNIX系统的进程
*          控制功能。可以理解为一个简化版的shell。
*/
void exec_command_like_shell(void) {
    const int MAXLINE = 1024;
    char commandBuffer[MAXLINE];

    //自定义命令提示符
    printf("%% ");

    //标准IO函数fgets从标准输入一次读取一行
    //当键入文件结束符（通常是Ctrl+D）时，fgets返回null指针
    while (fgets(commandBuffer, MAXLINE, stdin) != NULL) {

        //使用null字节替换换行符，用于execlp函数
        //execlp函数要求参数是以null结束的，而不是换行符
        if (commandBuffer[strlen(commandBuffer) - 1] == '\n') {
            commandBuffer[strlen(commandBuffer) - 1] = 0;
        }

        //fork创建一个新进程，返回两次
        //新进程是调用进程的一个副本，即子进程和父进程基本一样
        pid_t pid;
        if ((pid = fork()) < 0) {
            printf("fork error = %d\n", errno);
            continue;
        }

        //child，pid等于0表示子进程
        //fork和execlp函数的组合，即是常说的产生(spawn)一个新进程
        //子进程使用execlp函数用新的程序文件替换了原先执行的文件
        if (pid == 0) {
            execlp(commandBuffer, commandBuffer, (char *)0);
            printf("could not execute: %s\n", commandBuffer);
            exit(127);
        }

        //parent，pid大于0表示来到父进程，因此父进程知道子进程的ID
        //父进程希望等待子进程终止，status可以判断子进程是如何终止的
        int status = 0;
        if ((pid = waitpid(pid, &status, 0)) < 0) {
            printf("waitpid error = %d\n", errno);
        }

        //监听下一个命令
        printf("%% ");
    }

    exit(0);
}

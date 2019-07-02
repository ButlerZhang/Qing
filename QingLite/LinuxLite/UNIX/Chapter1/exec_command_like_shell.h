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

    //��׼IO����fgets�ӱ�׼����һ�ζ�ȡһ��
    while (fgets(command_buffer, MAXLINE, stdin) != NULL) {

        //ʹ��null�ֽ��滻���з�������execlp����
        if (command_buffer[strlen(command_buffer) - 1] == '\n') {
            command_buffer[strlen(command_buffer) - 1] = 0;
        }

        //fork����һ���½��̣���������
        pid_t pid;
        if ((pid = fork()) < 0) {
            printf("fork error\n");
            continue;
        }

        //child
        //fork��execlp��������ϣ����ǳ�˵�Ĳ���(spawn)һ���½���
        if (pid == 0) {
            execlp(command_buffer, command_buffer, (char*)0);
            printf("could not execute: %s\n", command_buffer);
            exit(127);
        }

        //parent
        //������ϣ���ȴ��ӽ�����ֹ��status�����ж��ӽ����������ֹ��
        int status = 0;
        if ((pid = waitpid(pid, &status, 0)) < 0) {
            printf("waitpid error\n");
        }

        printf("%% ");
    }

    exit(0);
}

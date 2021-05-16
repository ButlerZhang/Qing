#pragma once
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
* �������ܣ��ӱ�׼�����ȡ���Ȼ��ִ����Щ���
*          ������������Ҫ�����ǲ�������ִ�е�����
*          ���ݲ����������㹻����˵��UNIXϵͳ�Ľ���
*          ���ƹ��ܡ��������Ϊһ���򻯰��shell��
*/
void exec_command_like_shell(void) {
    const int MAXLINE = 1024;
    char commandBuffer[MAXLINE];

    //�Զ���������ʾ��
    printf("%% ");

    //��׼IO����fgets�ӱ�׼����һ�ζ�ȡһ��
    //�������ļ���������ͨ����Ctrl+D��ʱ��fgets����nullָ��
    while (fgets(commandBuffer, MAXLINE, stdin) != NULL) {

        //ʹ��null�ֽ��滻���з�������execlp����
        //execlp����Ҫ���������null�����ģ������ǻ��з�
        if (commandBuffer[strlen(commandBuffer) - 1] == '\n') {
            commandBuffer[strlen(commandBuffer) - 1] = 0;
        }

        //fork����һ���½��̣���������
        //�½����ǵ��ý��̵�һ�����������ӽ��̺͸����̻���һ��
        pid_t pid;
        if ((pid = fork()) < 0) {
            printf("fork error = %d\n", errno);
            continue;
        }

        //child��pid����0��ʾ�ӽ���
        //fork��execlp��������ϣ����ǳ�˵�Ĳ���(spawn)һ���½���
        //�ӽ���ʹ��execlp�������µĳ����ļ��滻��ԭ��ִ�е��ļ�
        if (pid == 0) {
            execlp(commandBuffer, commandBuffer, (char *)0);
            printf("could not execute: %s\n", commandBuffer);
            exit(127);
        }

        //parent��pid����0��ʾ���������̣���˸�����֪���ӽ��̵�ID
        //������ϣ���ȴ��ӽ�����ֹ��status�����ж��ӽ����������ֹ��
        int status = 0;
        if ((pid = waitpid(pid, &status, 0)) < 0) {
            printf("waitpid error = %d\n", errno);
        }

        //������һ������
        printf("%% ");
    }

    exit(0);
}

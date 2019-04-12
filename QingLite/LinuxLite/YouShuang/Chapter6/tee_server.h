#pragma once
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>



void tee_server(int argc, char* argv[])
{
    if (argc != 2) {
        printf("usage: %s <file>\n", argv[0]);
        return;
    }

    const int BUFFER_SIZE = 32768;

    int filefd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0666);
    assert(filefd > 0);

    int pipefd_stdout[2];
    int ret = pipe(pipefd_stdout);
    assert(ret != -1);

    int pipefd_file[2];
    ret = pipe(pipefd_file);
    assert(ret != -1);

    //����׼������������ܵ�pipefd_stdout
    __ssize_t result = splice(STDIN_FILENO, NULL, pipefd_stdout[1], NULL, BUFFER_SIZE, SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(result != -1);

    //���ܵ�pipefd_stdout��������Ƶ��ܵ�pipefd_file�������
    result = tee(               //�������ܵ��ļ�������֮�临�����ݣ�Ҳ���㿽������
        pipefd_stdout[0],       //Ҫ�������ݵĹܵ�
        pipefd_file[1],         //Ҫ������ݵĹܵ�
        BUFFER_SIZE,            //���ݳ���
        SPLICE_F_NONBLOCK);     //����������θ���
    assert(result != -1);

    //���ܵ�pipefd_file����������ļ�������filefd�ϣ��Ӷ�����׼���������д���ļ�
    result = splice(pipefd_file[0], NULL, filefd, NULL, BUFFER_SIZE, SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(result != -1);

    //���ܵ�pipefd_stdout��������򵽱�׼����������ݺ�д���ļ���������ȫһ��
    result = splice(pipefd_stdout[0], NULL, STDOUT_FILENO, NULL, BUFFER_SIZE, SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(result != -1);

    close(filefd);
    close(pipefd_file[0]);
    close(pipefd_file[1]);
    close(pipefd_stdout[0]);
    close(pipefd_stdout[1]);
}

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

    //将标准输入内容输入管道pipefd_stdout
    __ssize_t result = splice(STDIN_FILENO, NULL, pipefd_stdout[1], NULL, BUFFER_SIZE, SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(result != -1);

    //将管道pipefd_stdout的输出复制到管道pipefd_file的输入端
    result = tee(               //在两个管道文件描述符之间复制数据，也是零拷贝操作
        pipefd_stdout[0],       //要读入数据的管道
        pipefd_file[1],         //要输出数据的管道
        BUFFER_SIZE,            //数据长度
        SPLICE_F_NONBLOCK);     //控制数据如何复制
    assert(result != -1);

    //将管道pipefd_file的输出定向到文件描述符filefd上，从而将标准输入的内容写入文件
    result = splice(pipefd_file[0], NULL, filefd, NULL, BUFFER_SIZE, SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(result != -1);

    //将管道pipefd_stdout的输出定向到标准输出，其内容和写入文件的内容完全一致
    result = splice(pipefd_stdout[0], NULL, STDOUT_FILENO, NULL, BUFFER_SIZE, SPLICE_F_MORE | SPLICE_F_MOVE);
    assert(result != -1);

    close(filefd);
    close(pipefd_file[0]);
    close(pipefd_file[1]);
    close(pipefd_stdout[0]);
    close(pipefd_stdout[1]);
}

#pragma once
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "../../Qing/NetworkBase.h"



void select_server()
{
    NetworkBase Server;
    if (!Server.StartServer("192.168.3.126", 12345))
    {
        printf("start server failed.\n");
        return;
    }

    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    fd_set read_fds;                            //只处理读事件
    fd_set copy_read_fds;                       //read_fds的拷贝,read_fds可能会被修改导致循环错误
    FD_ZERO(&read_fds);                         //清零
    FD_SET(Server.GetSocket(), &read_fds);      //监控侦听socket

    while (true)
    {
        copy_read_fds = read_fds;
        int select_ret = select(                //在指定时间内侦听用户感兴趣的文件描述符上的可读可写和异常事件
            FD_SETSIZE,                         //指定被监听的文件描述符的总数，通常设置为所有文件描述符中的最大值加1
            &copy_read_fds,                     //可读文件描述符集合
            NULL,                               //可写文件描述符集合
            NULL,                               //异常事件文件描述符集合
            NULL);                              //超时时间，NULL表示一直阻塞，直到某个文件描述符就绪

        if (select_ret == 0)
        {
            printf("continue...\n");
            continue;
        }

        for (int index = 0; index < FD_SETSIZE; index++)
        {
            if (FD_ISSET(index, &copy_read_fds))
            {
                if (index == Server.GetSocket())
                {
                    int connfd = Server.Accept();
                    FD_SET(connfd, &read_fds);
                    printf("connected client socket = %d\n\n", connfd);
                }
                else
                {
                    memset(buffer, 0, BUFFER_SIZE);
                    ssize_t recv_ret = recv(index, buffer, BUFFER_SIZE - 1, 0);
                    if (recv_ret <= 0)
                    {
                        FD_CLR(index, &read_fds);
                        close(index);
                        printf("close client socket = %d\n\n", index);
                    }
                    else
                    {
                        printf("Socket = %d, Recv size = %d, message = %s\n", index, recv_ret, buffer);
                        ssize_t sendsize = send(index, "ACK", sizeof("ACK"), 0);
                        printf("Socket = %d, Send size = %d, message = ACK\n\n", index, sendsize);
                    }
                }
            }
        }
    }
}

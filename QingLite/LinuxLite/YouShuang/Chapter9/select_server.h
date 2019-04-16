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

    fd_set read_fds;                            //ֻ������¼�
    fd_set copy_read_fds;                       //read_fds�Ŀ���,read_fds���ܻᱻ�޸ĵ���ѭ������
    FD_ZERO(&read_fds);                         //����
    FD_SET(Server.GetSocket(), &read_fds);      //�������socket

    while (true)
    {
        copy_read_fds = read_fds;
        int select_ret = select(                //��ָ��ʱ���������û�����Ȥ���ļ��������ϵĿɶ���д���쳣�¼�
            FD_SETSIZE,                         //ָ�����������ļ���������������ͨ������Ϊ�����ļ��������е����ֵ��1
            &copy_read_fds,                     //�ɶ��ļ�����������
            NULL,                               //��д�ļ�����������
            NULL,                               //�쳣�¼��ļ�����������
            NULL);                              //��ʱʱ�䣬NULL��ʾһֱ������ֱ��ĳ���ļ�����������

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

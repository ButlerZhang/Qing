#pragma once
#include "demo_common.h"



void CallBack_InputFromCMD(int input, short events, void *arg)
{
    char Message[LIBEVENT_DEMO_BUFFER_SIZE];
    memset(Message, 0, LIBEVENT_DEMO_BUFFER_SIZE);

    ssize_t readsize = read(input, Message, sizeof(Message));
    if (readsize <= 0)
    {
        printf("can not read from cmd.\n");
    }
    else
    {
        Message[readsize-1] = '\0';
        int socket = *((int*)arg);
        ssize_t writesize = write(socket, Message, readsize);
        printf("send message = %s, size = %d.\n", Message, writesize);
    }
}

void CallBack_RecvFromServer(int socket, short events, void *arg)
{
    char Message[LIBEVENT_DEMO_BUFFER_SIZE];
    memset(Message, 0, LIBEVENT_DEMO_BUFFER_SIZE);

    ssize_t readsize = read(socket, Message, sizeof(Message));
    if (readsize <= 0)
    {
        printf("read form server failed.\n");
    }
    else
    {
        printf("recv message = %s, size = %d.\n\n", Message, readsize);
    }
}

void demo1_client()
{
    int ClientSocket = ConnectServer("192.168.3.126", 12345);
    if (ClientSocket <= 0)
    {
        printf("Connect Server failed.\n");
        return;
    }

    struct event_base *base = event_base_new();

    struct event *ev_sockfd = event_new(base, ClientSocket, EV_READ | EV_PERSIST, CallBack_RecvFromServer, NULL);
    event_add(ev_sockfd, NULL);

    struct event *ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_InputFromCMD, (void*)&ClientSocket);
    event_add(ev_cmd, NULL);

    printf("client start dispatch...\n\n");
    event_base_dispatch(base);
    event_base_free(base);
}

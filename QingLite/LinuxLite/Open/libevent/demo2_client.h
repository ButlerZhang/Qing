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
        Message[readsize - 1] = '\0';
        struct bufferevent *bev = (struct bufferevent*)arg;

        bufferevent_write(bev, Message, readsize);
        printf("send message = %s, size = %d.\n", Message, readsize);
    }
}

void CallBack_RecvFromServer(struct bufferevent *bev, void *arg)
{
    char Message[LIBEVENT_DEMO_BUFFER_SIZE];
    memset(Message, 0, LIBEVENT_DEMO_BUFFER_SIZE);

    size_t readsize = bufferevent_read(bev, Message, sizeof(Message));
    Message[readsize] = '\0';

    printf("recv message = %s, size = %d.\n\n", Message, readsize);
}

void CallBack_Event(struct bufferevent *bev, short event, void *arg)
{
    if (event &BEV_EVENT_EOF)
    {
        printf("connection closed.\n\n");
    }
    else if(event & BEV_EVENT_ERROR)
    {
        printf("unknow error.\n\n");
    }

    bufferevent_free(bev);
    event_free((struct event*)arg);
}

void demo2_client()
{
    int ClientSocket = ConnectServer("192.168.3.126", 12345);
    if (ClientSocket <= 0)
    {
        printf("Connect Server failed.\n");
        return;
    }

    struct event_base *base = event_base_new();
    struct bufferevent *bev = bufferevent_socket_new(base, ClientSocket, BEV_OPT_CLOSE_ON_FREE);
    struct event *ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_InputFromCMD, (void*)bev);

    event_add(ev_cmd, NULL);

    bufferevent_setcb(bev, CallBack_RecvFromServer, NULL, CallBack_Event, (void*)ev_cmd);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    printf("client start dispatch...\n");
    event_base_dispatch(base);
    event_base_free(base);
}

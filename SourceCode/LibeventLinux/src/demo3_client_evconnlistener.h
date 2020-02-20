#pragma once
#include "OS.h"
#include <event.h>



void CallBack3_InputFromCMD(evutil_socket_t Input, short events, void *arg)
{
    char Message[BUFFER_SIZE];
    memset(Message, 0, BUFFER_SIZE);

    int ReadSize = static_cast<int>(read(static_cast<int>(Input), Message, BUFFER_SIZE));
    if (ReadSize <= 0)
    {
        printf("ERROR: Can not read from cmd.\n\n");
        return;
    }

    Message[ReadSize - 1] = '\0';
    if (strlen(Message) <= 0)
    {
        return;
    }

    struct bufferevent *bev = (struct bufferevent*)arg;
    bufferevent_write(bev, Message, ReadSize);

    printf("Send message = %s, size = %d.\n", Message, ReadSize);
}

void CallBack3_RecvFromServer(struct bufferevent *bev, void *arg)
{
    char Message[BUFFER_SIZE];
    memset(Message, 0, BUFFER_SIZE);

    size_t RecvSize = bufferevent_read(bev, Message, BUFFER_SIZE);
    Message[RecvSize] = '\0';

    printf("Recv message = %s, size = %d.\n\n", Message, (int)RecvSize);
}

void CallBack3_ClientEvent(struct bufferevent *bev, short event, void *arg)
{
    if (event & BEV_EVENT_CONNECTED)
    {
        printf("Connected server succeed.\n\n");
        return;
    }
    else if (event & BEV_EVENT_EOF)
    {
        printf("Connection closed.\n\n");
    }
    else if (event & BEV_EVENT_ERROR)
    {
        printf("Unknow error.\n\n");
    }

    bufferevent_free(bev);
    event_free((struct event*)arg);
}

void demo3_client_evconnlistener(const char *ServerIP, int Port)
{
    struct event_base *base = event_base_new();
    struct bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

    struct event *ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack3_InputFromCMD, (void*)bev);
    event_add(ev_cmd, NULL);

    struct sockaddr_in ServerAddress;
    InitializeSocketAddress(ServerAddress, ServerIP, Port);
    bufferevent_socket_connect(bev, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));

    bufferevent_setcb(bev, CallBack3_RecvFromServer, NULL, CallBack3_ClientEvent, (void*)ev_cmd);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    printf("Client start dispatch...\n\n");
    event_base_dispatch(base);
    event_base_free(base);
}

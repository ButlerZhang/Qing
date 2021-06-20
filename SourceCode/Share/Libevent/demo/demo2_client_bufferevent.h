#pragma once
#include "../OS.h"
#include <event.h>



void CallBack2_InputFromCMD(evutil_socket_t Input, short events, void *arg)
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

void CallBack2_RecvFromServer(struct bufferevent *bev, void *arg)
{
    char Message[BUFFER_SIZE];
    memset(Message, 0, BUFFER_SIZE);

    size_t RecvSize = bufferevent_read(bev, Message, BUFFER_SIZE);
    Message[RecvSize] = '\0';

    printf("Recv message = %s, size = %I64d.\n\n", Message, RecvSize);
}

void CallBack2_ClientEvent(struct bufferevent *bev, short event, void *arg)
{
    if (event & BEV_EVENT_EOF)
    {
        printf("Connection closed.\n\n");
    }
    else if(event & BEV_EVENT_ERROR)
    {
        printf("ERROR: Unknow error.\n\n");
    }

    bufferevent_free(bev);
    event_free((struct event*)arg);
}

void demo2_client_bufferevent(const char *ServerIP, int Port)
{
    evutil_socket_t ClientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (ClientSocket <= -1)
    {
        printf("ERROR: Create socket failed.\n\n");
        return;
    }

    struct sockaddr_in ServerAddress;
    InitializeSocketAddress(ServerAddress, ServerIP, Port);
    if (connect(ClientSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) < 0)
    {
        printf("ERROR: Connected failed.\n\n");
        evutil_closesocket(ClientSocket);
        return;
    }

    evutil_make_socket_nonblocking(ClientSocket);

    struct event_base *base = event_base_new();
    struct bufferevent *bev = bufferevent_socket_new(base, ClientSocket, BEV_OPT_CLOSE_ON_FREE);
    struct event *ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack2_InputFromCMD, (void*)bev);

    event_add(ev_cmd, NULL);

    bufferevent_setcb(bev, CallBack2_RecvFromServer, NULL, CallBack2_ClientEvent, (void*)ev_cmd);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    printf("Client start dispatch...\n\n");
    event_base_dispatch(base);
    event_base_free(base);
}

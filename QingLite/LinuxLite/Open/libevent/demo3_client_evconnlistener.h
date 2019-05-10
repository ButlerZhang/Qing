#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <event.h>



void CallBack3_InputFromCMD(int Input, short events, void *arg)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    ssize_t ReadSize = read(Input, Message, sizeof(Message));
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
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    size_t RecvSize = bufferevent_read(bev, Message, sizeof(Message));
    Message[RecvSize] = '\0';

    printf("Recv message = %s, size = %d.\n\n", Message, RecvSize);
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
    bzero(&ServerAddress, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP, &(ServerAddress.sin_addr));
    ServerAddress.sin_port = htons(static_cast<uint16_t>(Port));
    bufferevent_socket_connect(bev, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));

    bufferevent_setcb(bev, CallBack3_RecvFromServer, NULL, CallBack3_ClientEvent, (void*)ev_cmd);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    printf("Client start dispatch...\n\n");
    event_base_dispatch(base);
    event_base_free(base);
}

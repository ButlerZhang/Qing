#pragma once
#include "OS.h"
#include <event.h>
#include <event2/listener.h>



void CallBack3_RecvClient(bufferevent *bev, void *arg)
{
    char Message[BUFFER_SIZE];
    memset(Message, 0, BUFFER_SIZE);

    size_t RecvSize = bufferevent_read(bev, Message, BUFFER_SIZE);
    Message[RecvSize] = '\0';

    evutil_socket_t ClientSocket = bufferevent_getfd(bev);
    printf("Client = %d recv message = %s, size = %d.\n", (int)ClientSocket, Message, (int)RecvSize);

    const std::string ACK("ACK");
    bufferevent_write(bev, ACK.c_str(), ACK.length());
    printf("Client = %d send ack, size = %d.\n\n", (int)ClientSocket, (int)ACK.length());
}

void CallBack3_ServerEvent(struct bufferevent *bev, short event, void *arg)
{
    evutil_socket_t ClientSocket = bufferevent_getfd(bev);
    if (event & BEV_EVENT_EOF)
    {
        printf("Client = %d connection closed.\n\n", (int)ClientSocket);
    }
    else if (event & BEV_EVENT_ERROR)
    {
        printf("Client = %d unknow error.\n\n", (int)ClientSocket);
    }

    bufferevent_free(bev);
}

void CallBack3_Listen(evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg)
{
    printf("Accept client socket = %d.\n\n", (int)fd);

    event_base *base = (event_base*)arg;
    bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, CallBack3_RecvClient, NULL, CallBack3_ServerEvent, NULL);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

void demo3_server_evconnlistener(const char *ServerIP, int Port)
{
    struct sockaddr_in BindAddress;
    InitializeSocketAddress(BindAddress, ServerIP, Port);

    event_base *base = event_base_new();
    evconnlistener *listener = evconnlistener_new_bind(
        base,
        CallBack3_Listen,
        base,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        10,
        (struct sockaddr*)&BindAddress,
        sizeof(BindAddress));

    printf("Server start dispatch...\n\n");
    event_base_dispatch(base);
    evconnlistener_free(listener);
    event_base_free(base);
}

#pragma once
#include "../OS.h"
#include <event.h>



void CallBack2_RecvClient(bufferevent *bev, void *arg)
{
    char Message[BUFFER_SIZE];
    memset(Message, 0, BUFFER_SIZE);

    size_t RecvSize = bufferevent_read(bev, Message, BUFFER_SIZE);
    Message[RecvSize] = '\0';

    evutil_socket_t ClientSocket = bufferevent_getfd(bev);
    printf("Client = %d recv message = %s, size = %I64d.\n", (int)ClientSocket, Message, RecvSize);

    const std::string ACK("ACK");
    bufferevent_write(bev, ACK.c_str(), ACK.length());
    printf("Client = %d send ack, size = %d.\n\n", (int)ClientSocket, (int)ACK.length());
}

void CallBack2_ServerEvent(struct bufferevent *bev, short event, void *arg)
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

void CallBack2_AcceptClient(evutil_socket_t ListenSocket, short events, void *arg)
{
    struct sockaddr_in ClientAddress;
    socklen_t AddressLength = sizeof(ClientAddress);

    evutil_socket_t ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientAddress, &AddressLength);
    evutil_make_socket_nonblocking(ClientSocket);

    printf("Accept client socket = %d.\n\n", (int)ClientSocket);

    struct event_base *base = (event_base*)arg;
    bufferevent *bev = bufferevent_socket_new(base, ClientSocket, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, CallBack2_RecvClient, NULL, CallBack2_ServerEvent, arg);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

void demo2_server_bufferevent(const char *ServerIP, int Port)
{
    evutil_socket_t ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket <= -1)
    {
        printf("ERROR: Create socket failed.\n\n");
        return;
    }

    evutil_make_listen_socket_reuseable(ListenSocket);

    struct sockaddr_in BindAddress;
    InitializeSocketAddress(BindAddress, ServerIP, Port);
    if (bind(ListenSocket, (sockaddr*)&BindAddress, sizeof(BindAddress)) < 0)
    {
        evutil_closesocket(ListenSocket);
        printf("ERROR: bind error.\n\n");
        return;
    }

    if (listen(ListenSocket, 5) < 0)
    {
        evutil_closesocket(ListenSocket);
        printf("ERROR: Listen error.\n\n");
        return;
    }

    evutil_make_socket_nonblocking(ListenSocket);

    struct event_base *base = event_base_new();

    struct event *ev_listen = event_new(base, ListenSocket, EV_READ | EV_PERSIST, CallBack2_AcceptClient, base);
    event_add(ev_listen, NULL);

    printf("Server start dispatch...\n\n");
    event_base_dispatch(base);
    event_base_free(base);
}

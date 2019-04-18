#pragma once
#include <string>
#include "demo_common.h"



void CallBack_RecvClient(bufferevent *bev, void *arg)
{
    char Message[LIBEVENT_DEMO_BUFFER_SIZE];
    memset(Message, 0, LIBEVENT_DEMO_BUFFER_SIZE);

    size_t len = bufferevent_read(bev, Message, sizeof(Message));
    Message[len] = '\0';

    int ClientSocket = bufferevent_getfd(bev);
    printf("client = %d recv message = %s, size = %d.\n", ClientSocket, Message, len);

    const std::string ACK("ACK");
    bufferevent_write(bev, ACK.c_str(), ACK.length());
    printf("client = %d recv send ack, size = %d.\n\n", ClientSocket, ACK.length());
}

void CallBack_Event(struct bufferevent *bev, short event, void *arg)
{
    int ClientSocket = bufferevent_getfd(bev);
    if (event &BEV_EVENT_EOF)
    {
        printf("client = %d connection closed.\n\n", ClientSocket);
    }
    else if (event & BEV_EVENT_ERROR)
    {
        printf("client = %d unknow error.\n\n", ClientSocket);
    }

    bufferevent_free(bev);
}

void CallBack_AcceptClient(int ListenSocket, short events, void *arg)
{
    struct sockaddr_in ClientAddress;
    socklen_t AddressLength = sizeof(ClientAddress);

    int ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientAddress, &AddressLength);
    evutil_make_socket_nonblocking(ClientSocket);

    printf("accept client socket = %d.\n\n", ClientSocket);

    struct event_base *base = (event_base*)arg;
    bufferevent *bev = bufferevent_socket_new(base, ClientSocket, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, CallBack_RecvClient, NULL, CallBack_Event, arg);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

void demo2_server()
{
    int ListenSocket = StartServer("192.168.3.126", 12345);
    if (ListenSocket == -1)
    {
        printf("Start Server failed.\n");
        return;
    }

    struct event_base *base = event_base_new();

    struct event *ev_listen = event_new(base, ListenSocket, EV_READ | EV_PERSIST, CallBack_AcceptClient, base);
    event_add(ev_listen, NULL);

    printf("server start dispatch...\n\n");
    event_base_dispatch(base);
    event_base_free(base);
}

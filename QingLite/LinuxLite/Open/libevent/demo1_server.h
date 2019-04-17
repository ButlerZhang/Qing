#pragma once
#include "demo_common.h"



void CallBack_RecvClient(int ClientSocket, short events, void *arg)
{
    char Message[4096];
    memset(Message, 0, sizeof(Message));

    ssize_t readsize = read(ClientSocket, Message, sizeof(Message));
    if (readsize <= 0)
    {
        printf("recv client message error.\n");
        event_free((struct event*)arg);
        close(ClientSocket);
    }
    else
    {
        printf("client = %d recv message = %s, size = %d.\n", ClientSocket, Message, readsize);

        const std::string ACK("ACK");
        ssize_t writesize = write(ClientSocket, ACK.c_str(), ACK.length());
        printf("client = %d recv send ack, size = %d.\n\n", ClientSocket, writesize);
    }
}

void CallBack_AcceptClient(int ListenSocket, short events, void *arg)
{
    struct sockaddr_in ClientAddress;
    socklen_t AddressLength = sizeof(ClientAddress);

    int ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientAddress, &AddressLength);
    evutil_make_socket_nonblocking(ClientSocket);

    printf("accept client socket = %d.\n\n", ClientSocket);

    struct event_base *base = (event_base*)arg;
    struct event *ev = event_new(NULL, -1, 0, NULL, NULL);

    event_assign(ev, base, ClientSocket, EV_READ | EV_PERSIST, CallBack_RecvClient, (void*)ev);
    event_add(ev, NULL);
}

void demo1_server()
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

    event_base_dispatch(base);
    printf("server start dispatch...\n");
}

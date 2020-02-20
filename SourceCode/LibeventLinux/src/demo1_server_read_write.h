#pragma once
#include "OS.h"
#include <event.h>



void CallBack1_RecvClient(evutil_socket_t ClientSocket, short events, void *arg)
{
    char Message[BUFFER_SIZE];
    memset(Message, 0, BUFFER_SIZE);

    int ReadSize = static_cast<int>(read(static_cast<int>(ClientSocket), Message, BUFFER_SIZE));
    if (ReadSize <= 0)
    {
        printf("ERROR: Recv client message error.\n\n");
        event_free((struct event*)arg);
        evutil_closesocket(ClientSocket);
    }
    else
    {
        printf("Client = %d recv message = %s, size = %d.\n", (int)ClientSocket, Message, ReadSize);

        const std::string ACK("ACK");
        int WriteSize = static_cast<int>(write(static_cast<int>(ClientSocket), ACK.c_str(), static_cast<int>(ACK.length())));
        printf("Client = %d send ack, size = %d.\n\n", (int)ClientSocket, WriteSize);
    }
}

void CallBack1_AcceptClient(evutil_socket_t ListenSocket, short events, void *arg)
{
    struct sockaddr_in ClientAddress;
    socklen_t AddressLength = sizeof(ClientAddress);

    evutil_socket_t ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientAddress, &AddressLength);
    evutil_make_socket_nonblocking(ClientSocket);

    printf("Accept client socket = %d.\n\n", (int)ClientSocket);

    struct event_base *base = (event_base*)arg;
    struct event *ev = event_new(NULL, -1, 0, NULL, NULL);

    event_assign(ev, base, ClientSocket, EV_READ | EV_PERSIST, CallBack1_RecvClient, (void*)ev);
    event_add(ev, NULL);
}

void demo1_server_read_write(const char *ServerIP, int Port)
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

    struct event *ev_listen = event_new(base, ListenSocket, EV_READ | EV_PERSIST, CallBack1_AcceptClient, base);
    event_add(ev_listen, NULL);

    printf("Server start dispatch...\n\n");
    event_base_dispatch(base);
    event_base_free(base);
}

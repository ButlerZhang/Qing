#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <event.h>
#include <string>



void CallBack1_RecvClient(int ClientSocket, short events, void *arg)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    ssize_t ReadSize = read(ClientSocket, Message, sizeof(Message));
    if (ReadSize <= 0)
    {
        printf("ERROR: Recv client message error.\n\n");
        event_free((struct event*)arg);
        close(ClientSocket);
    }
    else
    {
        printf("Client = %d recv message = %s, size = %d.\n", ClientSocket, Message, ReadSize);

        const std::string ACK("ACK");
        ssize_t WriteSize = write(ClientSocket, ACK.c_str(), ACK.length());
        printf("Client = %d send ack, size = %d.\n\n", ClientSocket, WriteSize);
    }
}

void CallBack1_AcceptClient(int ListenSocket, short events, void *arg)
{
    struct sockaddr_in ClientAddress;
    socklen_t AddressLength = sizeof(ClientAddress);

    int ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientAddress, &AddressLength);
    evutil_make_socket_nonblocking(ClientSocket);

    printf("Accept client socket = %d.\n\n", ClientSocket);

    struct event_base *base = (event_base*)arg;
    struct event *ev = event_new(NULL, -1, 0, NULL, NULL);

    event_assign(ev, base, ClientSocket, EV_READ | EV_PERSIST, CallBack1_RecvClient, (void*)ev);
    event_add(ev, NULL);
}

void demo1_server_read_write(const char *ServerIP, int Port)
{
    int ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket <= -1)
    {
        printf("ERROR: Create socket failed.\n\n");
        return;
    }

    evutil_make_listen_socket_reuseable(ListenSocket);

    struct sockaddr_in BindAddress;
    bzero(&BindAddress, sizeof(sockaddr_in));
    BindAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP, &(BindAddress.sin_addr));
    BindAddress.sin_port = htons(static_cast<uint16_t>(Port));

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

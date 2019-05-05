#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <event.h>
#include <string>



void CallBack2_RecvClient(bufferevent *bev, void *arg)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    size_t RecvSize = bufferevent_read(bev, Message, sizeof(Message));
    Message[RecvSize] = '\0';

    int ClientSocket = bufferevent_getfd(bev);
    printf("Client = %d recv message = %s, size = %d.\n", ClientSocket, Message, RecvSize);

    const std::string ACK("ACK");
    bufferevent_write(bev, ACK.c_str(), ACK.length());
    printf("Client = %d send ack, size = %d.\n\n", ClientSocket, ACK.length());
}

void CallBack2_ServerEvent(struct bufferevent *bev, short event, void *arg)
{
    int ClientSocket = bufferevent_getfd(bev);
    if (event & BEV_EVENT_EOF)
    {
        printf("Client = %d connection closed.\n\n", ClientSocket);
    }
    else if (event & BEV_EVENT_ERROR)
    {
        printf("Client = %d unknow error.\n\n", ClientSocket);
    }

    bufferevent_free(bev);
}

void CallBack2_AcceptClient(int ListenSocket, short events, void *arg)
{
    struct sockaddr_in ClientAddress;
    socklen_t AddressLength = sizeof(ClientAddress);

    int ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientAddress, &AddressLength);
    evutil_make_socket_nonblocking(ClientSocket);

    printf("Accept client socket = %d.\n\n", ClientSocket);

    struct event_base *base = (event_base*)arg;
    bufferevent *bev = bufferevent_socket_new(base, ClientSocket, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, CallBack2_RecvClient, NULL, CallBack2_ServerEvent, arg);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

void demo2_server_bufferevent(const char *ServerIP, int Port)
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

    struct event *ev_listen = event_new(base, ListenSocket, EV_READ | EV_PERSIST, CallBack2_AcceptClient, base);
    event_add(ev_listen, NULL);

    printf("Server start dispatch...\n\n");
    event_base_dispatch(base);
    event_base_free(base);
}

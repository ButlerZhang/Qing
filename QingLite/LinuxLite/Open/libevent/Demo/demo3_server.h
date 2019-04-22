#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <event.h>
#include <event2/listener.h>



void CallBack_RecvClient(bufferevent *bev, void *arg)
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

void CallBack_ServerEvent(struct bufferevent *bev, short event, void *arg)
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

void CallBack_Listen(evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg)
{
    printf("Accept client socket = %d.\n\n", fd);

    event_base *base = (event_base*)arg;
    bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, CallBack_RecvClient, NULL, CallBack_ServerEvent, NULL);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

void demo3_server(const char *ServerIP, int Port)
{
    struct sockaddr_in BindAddress;
    bzero(&BindAddress, sizeof(sockaddr_in));
    BindAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP, &(BindAddress.sin_addr));
    BindAddress.sin_port = htons(static_cast<uint16_t>(Port));

    event_base *base = event_base_new();
    evconnlistener *listener = evconnlistener_new_bind(
        base,
        CallBack_Listen,
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

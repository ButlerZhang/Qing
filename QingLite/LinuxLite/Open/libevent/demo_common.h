#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <event.h>
#include <string.h>



int ConnectServer(const char *ServerIP, int Port)
{
    struct sockaddr_in ServerAddress;
    bzero(&ServerAddress, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP, &(ServerAddress.sin_addr));
    ServerAddress.sin_port = htons(static_cast<uint16_t>(Port));

    int ClientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (ClientSocket <= -1)
    {
        printf("create socket failed.\n");
        return -1;
    }

    if (connect(ClientSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) < 0)
    {
        printf("connected failed.\n");
        close(ClientSocket);
        return -1;
    }

    return ClientSocket;
}

int StartServer(const char* ServerIP, int Port)
{
    int ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket <= -1)
    {
        printf("create socket failed.\n");
        return ListenSocket;
    }

    evutil_make_listen_socket_reuseable(ListenSocket);

    struct sockaddr_in BindAddress;
    BindAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP, &(BindAddress.sin_addr));
    BindAddress.sin_port = htons(static_cast<uint16_t>(Port));

    if (bind(ListenSocket, (sockaddr*)&BindAddress, sizeof(BindAddress)) < 0)
    {
        evutil_closesocket(ListenSocket);
        printf("bind error.\n");
        return -1;
    }

    if (listen(ListenSocket, 5) < 0)
    {
        evutil_closesocket(ListenSocket);
        printf("listen error.\n");
        return -1;
    }

    evutil_make_socket_nonblocking(ListenSocket);
    return ListenSocket;
}
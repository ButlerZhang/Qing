#pragma once
#include "../OS.h"
#include <event.h>



void CallBack1_InputFromCMD(evutil_socket_t Input, short events, void *arg)
{
    char Message[BUFFER_SIZE];
    memset(Message, 0, BUFFER_SIZE);

    int ReadSize = static_cast<int>(read(static_cast<int>(Input), Message, BUFFER_SIZE));
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

    int ServerSocket = *((int*)arg);
    int WriteSize = static_cast<int>(write(ServerSocket, Message, ReadSize));
    printf("Send message = %s, size = %d.\n", Message, WriteSize);
}

void CallBack1_RecvFromServer(evutil_socket_t ServerSocket, short events, void *arg)
{
    char Message[BUFFER_SIZE];
    memset(Message, 0, BUFFER_SIZE);

    int ReadSize = static_cast<int>(read(static_cast<int>(ServerSocket), Message, BUFFER_SIZE));
    if (ReadSize <= 0)
    {
        printf("ERROR: Recv form server failed.\n\n");
    }
    else
    {
        printf("Recv message = %s, size = %d.\n\n", Message, ReadSize);
    }
}

void demo1_client_read_write(const char *ServerIP, int Port)
{
    evutil_socket_t ClientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (ClientSocket <= -1)
    {
        printf("ERROR: Create socket failed.\n\n");
        return;
    }

    struct sockaddr_in ServerAddress;
    InitializeSocketAddress(ServerAddress, ServerIP, Port);
    if (connect(ClientSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) < 0)
    {
        printf("ERROR: Connected failed.\n\n");
        evutil_closesocket(ClientSocket);
        return;
    }

    evutil_make_socket_nonblocking(ClientSocket);

    struct event_base *base = event_base_new();

    struct event *ev_sockfd = event_new(base, ClientSocket, EV_READ | EV_PERSIST, CallBack1_RecvFromServer, NULL);
    event_add(ev_sockfd, NULL);

    struct event *ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack1_InputFromCMD, (void*)&ClientSocket);
    event_add(ev_cmd, NULL);

    printf("Client start dispatch...\n\n");
    event_base_dispatch(base);
    event_base_free(base);
}

#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <event.h>



void CallBack1_InputFromCMD(int Input, short events, void *arg)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    ssize_t ReadSize = read(Input, Message, sizeof(Message));
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
    ssize_t WriteSize = write(ServerSocket, Message, ReadSize);
    printf("Send message = %s, size = %d.\n", Message, WriteSize);
}

void CallBack1_RecvFromServer(int ServerSocket, short events, void *arg)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    ssize_t ReadSize = read(ServerSocket, Message, sizeof(Message));
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
    int ClientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (ClientSocket <= -1)
    {
        printf("ERROR: Create socket failed.\n\n");
        return;
    }

    struct sockaddr_in ServerAddress;
    bzero(&ServerAddress, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP, &(ServerAddress.sin_addr));
    ServerAddress.sin_port = htons(static_cast<uint16_t>(Port));

    if (connect(ClientSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) < 0)
    {
        printf("ERROR: Connected failed.\n\n");
        close(ClientSocket);
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

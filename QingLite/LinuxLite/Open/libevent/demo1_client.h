#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <event.h>
#include <event2/util.h>
#include <string>



int ConnectServerByTCP(const char *ServerIP, int Port)
{
    struct sockaddr_in ServerAddress;
    bzero(&ServerAddress, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP, &(ServerAddress.sin_addr));
    ServerAddress.sin_port = htons(static_cast<uint16_t>(Port));

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd <= -1)
    {
        printf("create socket failed.\n");
        return sockfd;
    }

    if (connect(sockfd, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) < 0)
    {
        printf("connected failed.\n");
        close(sockfd);
        return -1;
    }

    evutil_make_socket_nonblocking(sockfd);
    return sockfd;
}

void CallBack_CMDMessage(int fd, short events, void *arg)
{
    char msg[1024];
    ssize_t ret = read(fd, msg, sizeof(msg));
    if (ret <= 0)
    {
        printf("read failed.\n");
        return;
    }

    int sockfd = *((int*)arg);
    write(sockfd, msg, ret);
}

void CallBack_ClientRead(int fd, short events, void *arg)
{
    char msg[1024];

    ssize_t len = read(fd, msg, sizeof(msg) - 1);
    if (len <= 0)
    {
        printf("read failed.\n");
        return;
    }

    msg[len - 1] = '\0';
    printf("recv %s from server\n", msg);
}



void demo1_client()
{
    int sockfd = ConnectServerByTCP("192.168.3.126", 12345);
    if (sockfd <= -1)
    {
        printf("connect server failed.\n");
        return;
    }

    struct event_base *base = event_base_new();

    struct event *ev_sockfd = event_new(base, sockfd, EV_READ | EV_PERSIST, CallBack_ClientRead, NULL);
    event_add(ev_sockfd, NULL);

    struct event *ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_CMDMessage, (void*)&sockfd);
    event_add(ev_cmd, NULL);

    event_base_dispatch(base);
    printf("start dispatch...\n");
}

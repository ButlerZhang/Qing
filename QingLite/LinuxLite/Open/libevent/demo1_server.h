#pragma once
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <event.h>
#include <string>



int InitServerByTCP(int Port, int listen_num)
{
    evutil_socket_t listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener <= -1)
    {
        return listener;
    }

    evutil_make_listen_socket_reuseable(listener);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = 0;
    addr.sin_port = htons(static_cast<uint16_t>(Port));

    if (bind(listener, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        printf("bind error\n");
        evutil_closesocket(listener);
        return -1;
    }

    if (listen(listener, listen_num) < 0)
    {
        printf("listen error\n");
        evutil_closesocket(listener);
        return -1;
    }

    evutil_make_socket_nonblocking(listener);
    return listener;
}

void CallBack_ServerRead(int fd, short events, void *arg)
{
    char msg[4096];
    struct event *ev = (struct event*)arg;
    ssize_t len = read(fd, msg, sizeof(msg) - 1);

    if (len <= 0)
    {
        printf("read error\n");
        event_free(ev);
        close(fd);
        return;
    }

    msg[len] = '\0';
    printf("recv message = %s.\n", msg);

    char reply_msg[4096] = "I have received msg: ";
    strcat(reply_msg + strlen(reply_msg), msg);

    write(fd, reply_msg, strlen(reply_msg));
}

void CallBack_Accept(int fd, short events, void *arg)
{
    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    evutil_socket_t sockfd = accept(fd, (struct sockaddr*)&client, &len);
    evutil_make_socket_nonblocking(sockfd);

    printf("accept client socket = %d.\n", sockfd);

    struct event_base *base = (event_base*)arg;

    struct event *ev = event_new(NULL, -1, 0, NULL, NULL);

    event_assign(ev, base, sockfd, EV_READ | EV_PERSIST, CallBack_ServerRead, (void*)ev);
    event_add(ev, NULL);
}

void demo1_server()
{
    int listener = InitServerByTCP(12345, 10);
    if (listener == -1)
    {
        printf("server init error.\n");
        return;
    }

    struct event_base *base = event_base_new();

    struct event *ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, CallBack_Accept, base);
    event_add(ev_listen, NULL);

    event_base_dispatch(base);
}

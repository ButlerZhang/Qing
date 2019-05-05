#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <event.h>
#include <string>
#include <event2/event.h>
#include <event2/event_struct.h>



int g_UDPSocket = -1;
struct timeval g_LastSendTime;
struct sockaddr_in g_BroadcastAddress;

void CallBack4_TimeOut(evutil_socket_t fd, short event, void *arg)
{
    std::string Message("please connect 192.168.3.126:12345");
    ssize_t SendSize = sendto(g_UDPSocket,
        Message.c_str(), Message.length(),
        0, (struct sockaddr*)&g_BroadcastAddress, sizeof(g_BroadcastAddress));
    assert(SendSize == static_cast<ssize_t>(Message.length()));

    struct timeval NewTime, DifferentTime;
    evutil_gettimeofday(&NewTime, NULL);
    evutil_timersub(&NewTime, &g_LastSendTime, &DifferentTime);
    double elapsed = static_cast<double>(DifferentTime.tv_sec) + 
        (static_cast<double>(DifferentTime.tv_usec) / 1.0e6);

    printf("Broadcast = %s, %d: %.3f seconds elapsed.\n\n",
        Message.c_str(), (int)NewTime.tv_sec, elapsed);

    g_LastSendTime = NewTime;

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 10;
    event_add((struct event*)arg, &tv);
}

void demo4_server_udp_timer()
{
    g_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_UDPSocket == -1)
    {
        printf("ERROR: Create udp socket error.\n\n");
        return;
    }

    int iOptval = 1;
    if (setsockopt(g_UDPSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &iOptval, sizeof(int)) < 0)
    {
        printf("ERROR: setsockopt failed!");
        return;
    }

    bzero(&g_BroadcastAddress, sizeof(sockaddr_in));
    g_BroadcastAddress.sin_family = AF_INET;
    inet_pton(AF_INET, "255.255.255.255", &(g_BroadcastAddress.sin_addr));
    g_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(12345));

    struct event timeout;
    struct event_base *base = event_base_new();
    event_assign(&timeout, base, -1, EV_PERSIST, CallBack4_TimeOut, (void*)&timeout);

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 2;
    event_add(&timeout, &tv);

    evutil_gettimeofday(&g_LastSendTime, NULL);

    printf("Server start dispatch...\n\n");
    event_base_dispatch(base);
}

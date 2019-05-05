#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>



void demo4_client_udp_timer(void)
{
    int g_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
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

    struct sockaddr_in g_BroadcastAddress;
    bzero(&g_BroadcastAddress, sizeof(sockaddr_in));
    g_BroadcastAddress.sin_family = AF_INET;
    g_BroadcastAddress.sin_addr.s_addr = INADDR_ANY;
    g_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(12345));

    if (bind(g_UDPSocket, (struct sockaddr *)&g_BroadcastAddress, sizeof(g_BroadcastAddress)) == -1)
    {
        printf("bind failed!\n");
        return;
    }

    char Buffer[1024];
    memset(Buffer, 0, sizeof(Buffer));
    socklen_t AddressLength = sizeof(g_BroadcastAddress);

    while (true)
    {
        if (recvfrom(g_UDPSocket, Buffer, sizeof(Buffer), 0, (struct sockaddr *)&g_BroadcastAddress, &AddressLength) > 0)
        {
            printf("Recv:%s\n", Buffer);
        }
    }

    //close(g_UDPSocket);
}

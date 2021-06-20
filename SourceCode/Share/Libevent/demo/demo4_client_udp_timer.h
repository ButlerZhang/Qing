#pragma once
#include "../OS.h"
#include <event.h>



void demo4_client_udp_timer(const char *ServerIP, int Port)
{
    evutil_socket_t UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (UDPSocket <= -1)
    {
        printf("ERROR: Create udp socket error.\n\n");
        return;
    }

    int iOptval = 1;
    if (setsockopt(UDPSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, (const char*)&iOptval, sizeof(int)) < 0)
    {
        printf("ERROR: setsockopt failed!");
        return;
    }

    struct sockaddr_in BroadcastAddress;
    InitializeSocketAddress(BroadcastAddress, std::string(), Port);
    if (bind(UDPSocket, (struct sockaddr *)&BroadcastAddress, sizeof(BroadcastAddress)) == -1)
    {
        printf("bind failed!\n");
        return;
    }

    char Buffer[BUFFER_SIZE];
    memset(Buffer, 0, BUFFER_SIZE);
    socklen_t AddressLength = sizeof(BroadcastAddress);

    while (true)
    {
        if (recvfrom(UDPSocket, Buffer, sizeof(Buffer), 0, (struct sockaddr *)&BroadcastAddress, &AddressLength) > 0)
        {
            printf("Recv:%s\n", Buffer);
        }
    }

    evutil_closesocket(UDPSocket);
}

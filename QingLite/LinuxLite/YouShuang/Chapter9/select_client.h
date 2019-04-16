#pragma once
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "../../Qing/NetworkBase.h"



void select_client()
{
    NetworkBase Client;
    if (!Client.StartClient() || !Client.ConnectServer("192.168.3.126", 12345))
    {
        printf("start client failed.\n");
        return;
    }

    const int BUFFER_SIZE = 1024;
    char Buffer[BUFFER_SIZE];

    while (true)
    {
        std::cout << "Input message: ";
        memset(Buffer, 0, BUFFER_SIZE);
        std::cin >> Buffer;

        ssize_t sendsize = send(Client.GetSocket(), Buffer, strlen(Buffer), 0);
        printf("Send size = %d, message = %s\n", sendsize, Buffer);

        memset(Buffer, 0, BUFFER_SIZE);
        ssize_t recvsize = recv(Client.GetSocket(), Buffer, BUFFER_SIZE - 1, 0);
        printf("Recv size = %d, message = %s\n\n", recvsize, Buffer);
    }
}

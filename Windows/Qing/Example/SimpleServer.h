#pragma once
#include <Windows.h>



DWORD WINAPI ThreadFunc(void*)
{
    return 0;
}

int my_simple_server_test_main()
{
    WSAData wsadata;
    SOCKADDR ServerAddr, ClientAddr;

    WSAStartup(MAKEWORD(2, 2), &wsadata);
    SOCKET ListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    bind(ListeningSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));

    listen(ListeningSocket, 5);
    int nlistenAddrLen = sizeof(ClientAddr);
    while (TRUE)
    {
        DWORD dwThreadID;
        SOCKET NewConnection = accept(ListeningSocket, (SOCKADDR*)&ClientAddr, &nlistenAddrLen);
        HANDLE hThread = CreateThread(NULL, 0, ThreadFunc, (void*)NewConnection, 0, &dwThreadID);
        CloseHandle(hThread);
    }

    return 0;
}
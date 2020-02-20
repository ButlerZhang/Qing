#pragma once
#ifdef _WIN32
#include <io.h>
#include <WS2tcpip.h>
#include <winsock.h>                    //sockaddr_in
#include <sys/types.h>

#define STDIN_FILENO 0

#else
#include <sys/socket.h>
#include <arpa/inet.h>                  //sockaddr_in
#include <unistd.h>
#include <string.h>                     //memset
#endif

#include <string>


inline void InitializeSocketAddress(sockaddr_in &SocketAddress, const std::string &IP, int Port)
{
    memset(&SocketAddress, 0, sizeof(sockaddr_in));

    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_port = htons(static_cast<uint16_t>(Port));

    if (!IP.empty())
    {
        inet_pton(AF_INET, IP.c_str(), &(SocketAddress.sin_addr));
    }
    else
    {
        SocketAddress.sin_addr.s_addr = INADDR_ANY;
    }
}
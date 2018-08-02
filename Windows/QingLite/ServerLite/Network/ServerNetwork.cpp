#include "ServerNetwork.h"



ServerNetwork::ServerNetwork() : Network()
{
}

ServerNetwork::~ServerNetwork()
{
}

bool ServerNetwork::Start(const std::string & IP, int Port)
{
    if (!CreateSocket())
    {
        return false;
    }

    if (!BindSocket(IP, Port))
    {
        return false;
    }

    if (!StartListen())
    {
        return false;
    }

    return true;
}

bool ServerNetwork::BindSocket(const std::string &IP, int Port)
{
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP.c_str());
    addr.sin_port = htons(Port);

    int error = bind(m_Socket, (LPSOCKADDR)&addr, sizeof(addr));
    return (error == 0);
}

bool ServerNetwork::StartListen(int backlog)
{
    return (listen(m_Socket, backlog) == 0);
}

bool ServerNetwork::AcceptConnect()
{
    fd_set readset;
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_ZERO(&readset);
    FD_SET(m_Socket, &readset);

    int ret = select(FD_SETSIZE, &readset, NULL, NULL, &timeout);
    if (ret > 0 && FD_ISSET(m_Socket, &readset))
    {
        SOCKADDR addr;
        int len = sizeof(addr);
        SOCKET tempSocket;
        tempSocket = accept(m_Socket, (SOCKADDR*)&addr, (int*)&len);
        if (tempSocket == INVALID_SOCKET)
        {
            //¡¨Ω” ß∞‹
        }
    }

    return false;
}

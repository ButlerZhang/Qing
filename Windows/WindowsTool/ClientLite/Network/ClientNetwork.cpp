#include "ClientNetwork.h"



ClientNetwork::ClientNetwork() : Network()
{
}

ClientNetwork::~ClientNetwork()
{
}

bool ClientNetwork::BindSocket()
{
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(0);

    int error = bind(m_Socket, (LPSOCKADDR)&addr, sizeof(addr));
    return (error == 0);
}

bool ClientNetwork::Connect(const std::string & IP, int Port)
{
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Port);
    addr.sin_addr.s_addr = inet_addr(IP.c_str());
    int ret = connect(m_Socket, (SOCKADDR*)&addr, sizeof(addr));
    return (ret == 0);
}

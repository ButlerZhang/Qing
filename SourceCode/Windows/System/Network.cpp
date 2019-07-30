#include "Network.h"
#include "Network/NetworkClient.h"
#include "Network/NetworkServer.h"



Network::Network()
{
}

Network::~Network()
{
}

bool Network::Start(const std::string &ServerIP, int Port, bool IsServer)
{
    if (IsServer)
    {
        m_Network = std::make_shared<NetworkServer>();
    }
    else
    {
        m_Network = std::make_shared<NetworkClient>();
    }

    return m_Network->Start(ServerIP, Port);
}

void Network::Stop()
{
    if (m_Network != NULL)
    {
        m_Network->Stop();
    }
}

int Network::Send(const void * MessageData, int MessageSize)
{
    return m_Network->Send(MessageData, MessageSize);
}

int Network::Send(SOCKET ClientID, const void * MessageData, int MessageSize, __int64 Timeout)
{
    return m_Network->Send(ClientID, MessageData, MessageSize, Timeout);
}

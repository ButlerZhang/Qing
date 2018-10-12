#include "..\HeaderFiles\QingNetwork.h"
#include "..\Model\Network\NetworkClient.h"
#include "..\Model\Network\NetworkServer.h"

//#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN



QingNetwork::QingNetwork()
{
}

QingNetwork::~QingNetwork()
{
}

bool QingNetwork::Start(const std::string &ServerIP, int Port, bool IsServer)
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

void QingNetwork::Stop()
{
    if (m_Network != NULL)
    {
        m_Network->Stop();
    }
}

int QingNetwork::Send(const void * MessageData, int MessageSize)
{
    return m_Network->Send(MessageData, MessageSize);
}

int QingNetwork::Send(unsigned __int64 NaturalIndex, const void * MessageData, int MessageSize, __int64 Timeout)
{
    return m_Network->Send(NaturalIndex, MessageData, MessageSize, Timeout);
}

QING_NAMESPACE_END
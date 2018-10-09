#include "NetworkBase.h"
#include "..\..\HeaderFiles\QingLog.h"
#include "..\..\HeaderFiles\LocalComputer.h"
#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN



NetworkBase::NetworkBase() : m_ServerListenPort(0)
{
}

NetworkBase::~NetworkBase()
{
}

bool NetworkBase::Start(const std::string & ServerIP, int Port)
{
    m_ServerIP = ServerIP;
    m_ServerListenPort = Port;
    return true;
}

const std::string & NetworkBase::GetLocalIP()
{
    if (m_ServerIP.empty())
    {
        LocalComputer MyComputer;
        std::vector<std::string> IPVector;
        if (MyComputer.GetIPAddress(IPVector))
        {
            m_ServerIP = IPVector[0];
        }
    }

    return m_ServerIP;
}

void NetworkBase::ReleaseHandle(HANDLE & Handle)
{
    if (Handle != NULL && Handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(Handle);
        Handle = NULL;
    }
}

void NetworkBase::FillAddress(sockaddr_in &ServerAddress)
{
    ZeroMemory((char*)&ServerAddress, sizeof(ServerAddress));

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(m_ServerListenPort);

    if (m_ServerIP.empty())
    {
        ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        QingLog::Write(LL_INFO, "Fill address set IP = %s, Port = %d.", "INADDR_ANY", m_ServerListenPort);
    }
    else
    {
        inet_pton(AF_INET, m_ServerIP.c_str(), &(ServerAddress.sin_addr.s_addr));
        QingLog::Write(LL_INFO, "Fill address set IP = %s, Port = %d.", m_ServerIP.c_str(), m_ServerListenPort);
    }
}

QING_NAMESPACE_END
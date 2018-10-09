#include "NetworkBase.h"
#include "..\..\HeaderFiles\LocalComputer.h"

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

QING_NAMESPACE_END
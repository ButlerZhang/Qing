#pragma once
#include "..\..\HeaderFiles\QingBase.h"
#include <winsock2.h>
#include <MSWSock.h>
#include <string>

QING_NAMESPACE_BEGIN



class NetworkBase
{
public:

    NetworkBase();
    virtual ~NetworkBase();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual void Stop() = 0;

    const std::string& GetLocalIP();
    int GetServerPort() const { return m_ServerListenPort; }

protected:

    void ReleaseHandle(HANDLE &Handle);
    void FillAddress(sockaddr_in &ServerAddress);

protected:

    int                                     m_ServerListenPort;
    std::string                             m_ServerIP;
};

QING_NAMESPACE_END
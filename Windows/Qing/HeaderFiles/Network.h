#pragma once
#include "..\Model\Network\QingServer.h"
#include "..\Model\Network\QingClient.h"
#include <WinSock2.h>
#include <string>

QING_NAMESPACE_BEGIN



bool QING_DLL StartupNetwork();
bool QING_DLL ShutdownNetwork();



class QING_DLL Network
{
public:

    Network();
    virtual ~Network();

    bool Start(bool IsServer);
    void Stop();

private:

    std::shared_ptr<QingClient> m_Client;
    std::shared_ptr<QingServer> m_Server;

};

QING_NAMESPACE_END
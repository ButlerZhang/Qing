#pragma once
#include "QingBase.h"
#include "..\Model\Network\QingServer.h"
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

    bool Start();
    void Stop();

private:

    QingServer    m_IOCP;
};

QING_NAMESPACE_END
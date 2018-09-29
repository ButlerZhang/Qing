#pragma once
#include "QingBase.h"
#include "..\Model\Network\QingIOCP.h"
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

    QingIOCP    m_IOCP;
};

QING_NAMESPACE_END
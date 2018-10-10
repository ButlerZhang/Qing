#pragma once
#include "..\Model\Network\NetworkBase.h"
#include <memory>

QING_NAMESPACE_BEGIN

bool QING_DLL StartupNetwork();
bool QING_DLL ShutdownNetwork();



class QING_DLL Network
{
public:

    Network();
    virtual ~Network();

    bool Start(const std::string &ServerIP, int Port, bool IsServer);
    void Stop();

    int Send(const void *MessageData, int MessageSize);
    int Send(int NaturalIndex, const void *MessageData, int MessageSize, __int64 Timeout = 0);

private:

    std::shared_ptr<NetworkBase> m_Network;
};

QING_NAMESPACE_END
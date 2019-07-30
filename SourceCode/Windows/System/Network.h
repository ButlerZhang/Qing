#pragma once
#include "Network/NetworkBase.h"
#include <memory>



class Network
{
public:

    Network();
    virtual ~Network();

    bool Start(const std::string &ServerIP, int Port, bool IsServer);
    void Stop();

    bool IsRunning() const { return m_Network->IsRunning(); }
    int  Send(const void *MessageData, int MessageSize);
    int  Send(SOCKET ClientID, const void *MessageData, int MessageSize, __int64 Timeout = 0);

private:

    std::shared_ptr<NetworkBase> m_Network;
};

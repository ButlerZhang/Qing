#pragma once
#include "../../core/network/NetworkBase.h"



class TCPHandler
{
public:

    TCPHandler();
    virtual ~TCPHandler();
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg);

private:

    bool ProcessRandom(NetworkMessage &NetworkMsg);
    bool ProcessLogin(NetworkMessage &NetworkMsg);
    bool ProcessLogout(NetworkMessage &NetworkMsg);
};

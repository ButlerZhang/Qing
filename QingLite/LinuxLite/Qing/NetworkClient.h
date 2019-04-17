#pragma once
#include "NetworkBase.h"



class NetworkClient : public NetworkBase
{
public:

    NetworkClient();
    ~NetworkClient();

    bool StartClient(const std::string &IP, int Port);
    bool ConnectServer(const std::string &IP, int Port);

private:

    bool m_IsConnected;
};

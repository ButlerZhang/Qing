#pragma once
#include "NetworkBase.h"



class NetworkServer : public NetworkBase
{
public:

    NetworkServer();
    virtual ~NetworkServer();

    int  Accept();
    bool StartServer(const std::string &IP, int Port);

private:

    bool BindSocket(const std::string &IP, int Port);
    bool ListenSocket();
};

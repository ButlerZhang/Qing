#pragma once
#include "QNetworkBase.h"



class NetworkServer : public QNetworkBase
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

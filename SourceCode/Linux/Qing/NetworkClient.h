#pragma once
#include "QNetworkBase.h"



class NetworkClient : public QNetworkBase
{
public:

    NetworkClient();
    ~NetworkClient();

    bool StartClient(const std::string &IP, int Port);
    bool ConnectServer(const std::string &IP, int Port);

private:

    bool m_IsConnected;
};

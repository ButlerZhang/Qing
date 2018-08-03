#pragma once
#include "..\..\..\Qing\HeaderFiles\Network.h"



class ServerNetwork : public Qing::Network
{
public:

    ServerNetwork();
    ~ServerNetwork();

    bool Start(const std::string &IP, int Port);
    SOCKET GetConnectedClient();
};

void TestServerNetwork();
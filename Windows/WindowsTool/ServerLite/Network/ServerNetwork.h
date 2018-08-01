#pragma once
#include "..\..\..\Qing\HeaderFiles\Network.h"
#include <string>



class ServerNetwork : public Qing::Network
{
public:

    ServerNetwork();
    ~ServerNetwork();

    bool Start(const std::string &IP, int Port);

private:

    bool BindSocket(const std::string &IP, int Port);
    bool StartListen(int backlog = SOMAXCONN);
    bool AcceptConnect();
};

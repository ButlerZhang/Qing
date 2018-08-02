#pragma once
#include "..\..\..\Qing\HeaderFiles\Network.h"
#include <string>



class ClientNetwork : public Qing::Network
{
public:

    ClientNetwork();
    ~ClientNetwork();

protected:

    bool BindSocket();
    bool Connect(const std::string &IP, int Port);
};

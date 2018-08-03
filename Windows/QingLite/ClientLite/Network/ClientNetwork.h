#pragma once
#include "..\..\..\Qing\HeaderFiles\Network.h"



class ClientNetwork : public Qing::Network
{
public:

    ClientNetwork();
    ~ClientNetwork();

    bool IsConnected() const { return m_IsConnected; }
    bool ConnectServer(const std::string &IP, int Port);

    bool RecvData(char *Buffer, int BufferSize);
    bool SendData(const char *Buffer, int BufferSize);

private:

    bool m_IsConnected;
};

void TestClientNetwork();
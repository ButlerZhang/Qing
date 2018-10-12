#pragma once
#include "..\..\..\Qing\HeaderFiles\QingNetwork.h"



class ClientNetwork
{
public:

    ClientNetwork(int ClientCount);
    ~ClientNetwork();

    bool Start(const std::string &ServerIP, int Port, bool IsServer);
    void Stop();

    void TestClient();
    static DWORD WINAPI CallBack_ClientSendRandomMessageThread(LPVOID lpParam);

private:

    std::vector<std::shared_ptr<Qing::QingNetwork>> m_ClientVector;
};

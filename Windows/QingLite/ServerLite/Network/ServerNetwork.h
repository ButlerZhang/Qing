#pragma once
#include "..\..\..\Qing\HeaderFiles\Network.h"



class ServerNetwork : public Qing::Network
{
public:

    ServerNetwork();
    ~ServerNetwork();

    void TestServer();
    static DWORD WINAPI CallBack_ServerSendRandomMessageThread(LPVOID lpParam);
};

#pragma once
#include "..\..\..\Qing\HeaderFiles\QingNetwork.h"



class ServerNetwork : public Qing::QingNetwork
{
public:

    ServerNetwork();
    ~ServerNetwork();

    void TestServer();
    static DWORD WINAPI CallBack_ServerSendRandomMessageThread(LPVOID lpParam);
};

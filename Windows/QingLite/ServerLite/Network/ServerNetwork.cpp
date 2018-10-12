#include "ServerNetwork.h"
#include "..\..\..\Qing\HeaderFiles\Utility.h"
#include "..\..\..\Qing\HeaderFiles\RandomGenerator.h"

#include <iostream>



ServerNetwork::ServerNetwork() : QingNetwork()
{
}

ServerNetwork::~ServerNetwork()
{
}

void ServerNetwork::TestServer()
{
    DWORD nThreadID;
    ::CreateThread(0, 0, CallBack_ServerSendRandomMessageThread, (void*)this, 0, &nThreadID);
}

DWORD ServerNetwork::CallBack_ServerSendRandomMessageThread(LPVOID lpParam)
{
    Qing::RandomGenerator MyRandom;
    ServerNetwork *MyServer = (ServerNetwork*)lpParam;

    while (MyServer->IsRunning())
    {
        Sleep(MyRandom.GetRandomUIntInRange(1000, 10000));

        std::string RandomMessage(Qing::GetGUID());
        MyServer->Send(0, RandomMessage.c_str(), static_cast<int>(RandomMessage.size()));
    }

    return 0;
}

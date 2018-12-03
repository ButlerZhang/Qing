#include "ServerNetwork.h"
#include "..\..\..\Qing\SourceCode\System\SystemShare.h"
#include "..\..\..\Qing\SourceCode\Standard\RandomGenerator.h"

#include <iostream>



ServerNetwork::ServerNetwork() : Network()
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

        std::wstring RandomMessage(Qing::GetGUID());
        MyServer->Send(0, RandomMessage.c_str(), static_cast<int>(RandomMessage.size()));
    }

    return 0;
}

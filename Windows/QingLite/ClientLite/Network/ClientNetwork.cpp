#include "ClientNetwork.h"
#include "..\..\..\Qing\HeaderFiles\Utility.h"
#include "..\..\..\Qing\HeaderFiles\RandomGenerator.h"



ClientNetwork::ClientNetwork(int ClientCount)
{
    for (int Index = 0; Index < ClientCount; Index++)
    {
        m_ClientVector.push_back(std::make_shared<Qing::QingNetwork>());
    }
}

ClientNetwork::~ClientNetwork()
{
    m_ClientVector.clear();
}

bool ClientNetwork::Start(const std::string & ServerIP, int Port, bool IsServer)
{
    for (auto Index = 0; Index != m_ClientVector.size(); Index++)
    {
        m_ClientVector[Index]->Start(ServerIP, Port, false);
    }

    return true;
}

void ClientNetwork::Stop()
{
    for (auto Index = 0; Index != m_ClientVector.size(); Index++)
    {
        m_ClientVector[Index]->Stop();
    }
}

void ClientNetwork::TestClient()
{
    DWORD nThreadID;
    ::CreateThread(0, 0, CallBack_ClientSendRandomMessageThread, (void*)this, 0, &nThreadID);
}

DWORD ClientNetwork::CallBack_ClientSendRandomMessageThread(LPVOID lpParam)
{
    Qing::RandomGenerator MyRandom;
    ClientNetwork *MyServer = (ClientNetwork*)lpParam;

    while (!MyServer->m_ClientVector.empty())
    {
        Sleep(MyRandom.GetRandomUIntInRange(1000, 10000));

        int StopCount = 0;
        for (auto Index = 0; Index != MyServer->m_ClientVector.size(); Index++)
        {
            if (!MyServer->m_ClientVector[Index]->IsRunning())
            {
                if (++StopCount == static_cast<int>(MyServer->m_ClientVector.size()))
                {
                    return 0;
                }

                continue;
            }

            std::string RandomMessage(Qing::GetGUID());
            MyServer->m_ClientVector[Index]->Send(RandomMessage.c_str(), static_cast<int>(RandomMessage.size()));
        }
    }

    return 0;
}

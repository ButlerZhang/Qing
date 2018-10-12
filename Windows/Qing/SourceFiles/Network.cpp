#include "..\HeaderFiles\Network.h"
#include "..\HeaderFiles\QingLog.h"
#include "..\Model\Network\NetworkClient.h"
#include "..\Model\Network\NetworkServer.h"

#include "..\HeaderFiles\Utility.h"
#include "..\HeaderFiles\RandomGenerator.h"

#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN



long g_NetworkInitCounter = 0;

bool QING_DLL StartupNetwork()
{
    //ԭ�Ӽӣ���ֻ֤����һ��WSAStartup
    if (InterlockedIncrement(&g_NetworkInitCounter) == 1)
    {
        WSAData wsaData;
        //WSAStartup������ʼ��Windows Sockets DLL���ɹ�����ܵ�������API
        //MAKEWORD(2,2)��ʾʹ�õ���WinSocket2.2�汾
        int Result = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (Result == NO_ERROR)
        {
            return true;
        }

        QingLog::Write(LL_ERROR, "WSAStartup error = %d.", Result);
    }

    return false;
}

bool QING_DLL ShutdownNetwork()
{
    //ԭ�Ӽ�����ֻ֤����һ��WSACleanup
    if (InterlockedDecrement(&g_NetworkInitCounter) == 0)
    {
        //��ֹWinsock 2 DLL��ʹ��
        int Result = ::WSACleanup();
        if (Result == NO_ERROR)
        {
            return true;
        }

        QingLog::Write(LL_ERROR, "WSACleanup error = %d.", Result);
    }

    return false;
}



Network::Network()
{
}

Network::~Network()
{
}

bool Network::Start(const std::string &ServerIP, int Port, bool IsServer)
{
    if (IsServer)
    {
        m_Network = std::make_shared<NetworkServer>();
    }
    else
    {
        m_Network = std::make_shared<NetworkClient>();
    }

    return m_Network->Start(ServerIP, Port);
}

void Network::Stop()
{
    if (m_Network != NULL)
    {
        m_Network->Stop();
    }
}

int Network::Send(const void * MessageData, int MessageSize)
{
    return m_Network->Send(MessageData, MessageSize);
}

int Network::Send(unsigned __int64 NaturalIndex, const void * MessageData, int MessageSize, __int64 Timeout)
{
    return m_Network->Send(NaturalIndex, MessageData, MessageSize, Timeout);
}

void Network::TestServer()
{
    DWORD nThreadID;
    ::CreateThread(0, 0, CallBack_RandomMessageThread, (void*)this, 0, &nThreadID);
}

DWORD Network::CallBack_RandomMessageThread(LPVOID lpParam)
{
    Qing::RandomGenerator MyRandom;
    Network *MyServer = (Network*)lpParam;

    while (MyServer->IsRunning())
    {
        Sleep(MyRandom.GetRandomUIntInRange(1000, 10000));

        std::string RandomMessage(Qing::GetGUID());
        MyServer->Send(0, RandomMessage.c_str(), static_cast<int>(RandomMessage.size()));
    }

    return 0;
}

QING_NAMESPACE_END
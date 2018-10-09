#include "..\HeaderFiles\Network.h"
#include "..\HeaderFiles\QingLog.h"
#include "..\Model\Network\QingClient.h"
#include "..\Model\Network\QingServer.h"
#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN



long g_NetworkInitCounter = 0;

bool QING_DLL StartupNetwork()
{
    //原子加，保证只调用一次WSAStartup
    if (InterlockedIncrement(&g_NetworkInitCounter) == 1)
    {
        WSAData wsaData;
        //WSAStartup用来初始化Windows Sockets DLL，成功后才能调用其它API
        //MAKEWORD(2,2)表示使用的是WinSocket2.2版本
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
    //原子减，保证只调用一次WSACleanup
    if (InterlockedDecrement(&g_NetworkInitCounter) == 0)
    {
        //终止Winsock 2 DLL的使用
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
        m_Network = std::make_shared<QingServer>();
    }
    else
    {
        m_Network = std::make_shared<QingClient>();
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

QING_NAMESPACE_END
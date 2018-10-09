#include "..\HeaderFiles\Network.h"
#include "..\HeaderFiles\QingLog.h"
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

bool Network::Start(bool IsServer)
{
    if (IsServer)
    {
        m_Server = std::make_shared<QingServer>();
        return m_Server->Start(12345);
    }
    else
    {
        m_Client = std::make_shared<QingClient>();
        return m_Client->Start("192.168.3.168", 12345);
    }
}

void Network::Stop()
{
    if (m_Server != NULL)
    {
        m_Server->Stop();
    }

    if (m_Client != NULL)
    {
        m_Client->Stop();
    }
}

QING_NAMESPACE_END
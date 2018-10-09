#include "NetworkBase.h"
#include "..\..\HeaderFiles\QingLog.h"
#include "..\..\HeaderFiles\LocalComputer.h"
#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN



NetworkBase::NetworkBase() : m_ServerListenPort(0),
                             m_hIOCompletionPort(NULL)
{
}

NetworkBase::~NetworkBase()
{
}

bool NetworkBase::Start(const std::string & ServerIP, int Port)
{
    m_ServerIP = ServerIP;
    m_ServerListenPort = Port;
    return true;
}

const std::string & NetworkBase::GetLocalIP()
{
    if (m_ServerIP.empty())
    {
        LocalComputer MyComputer;
        std::vector<std::string> IPVector;
        if (MyComputer.GetIPAddress(IPVector))
        {
            m_ServerIP = IPVector[0];
        }
    }

    return m_ServerIP;
}

void NetworkBase::ReleaseSocket(SOCKET Socket)
{
    if (Socket != INVALID_SOCKET)
    {
        ::shutdown(Socket, SD_BOTH);
        ::closesocket(Socket);
    }
}

void NetworkBase::ReleaseHandle(HANDLE & Handle)
{
    if (Handle != NULL && Handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(Handle);
        Handle = NULL;
    }
}

void NetworkBase::SetSocketLinger(SOCKET Socket)
{
    struct Linger
    {
        u_short Switch;
        u_short Time;
    };

    Linger linger = { 1,0 };
    setsockopt(Socket, SOL_SOCKET, SO_LINGER, (const char*)&linger, sizeof(Linger));
}

void NetworkBase::FillAddress(sockaddr_in &ServerAddress)
{
    ZeroMemory((char*)&ServerAddress, sizeof(ServerAddress));

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(m_ServerListenPort);

    if (m_ServerIP.empty())
    {
        ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        QingLog::Write(LL_INFO, "Fill address set IP = %s, Port = %d.", "INADDR_ANY", m_ServerListenPort);
    }
    else
    {
        inet_pton(AF_INET, m_ServerIP.c_str(), &(ServerAddress.sin_addr.s_addr));
        QingLog::Write(LL_INFO, "Fill address set IP = %s, Port = %d.", m_ServerIP.c_str(), m_ServerListenPort);
    }
}

bool NetworkBase::CreateIOCP()
{
    m_hIOCompletionPort = CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,   //FileHandle是有效的文件句柄或INVALID_HANDLE_VALUE
        NULL,                   //ExistingCompletionPort已经存在的完成端口，如果为NULL则新建一个
        0,                      //CompletionKey是传送给处理函数的参数
        0);                     //NumberOfConcurrentThreads有多少个线程在访问这个消息队列
                                //如果设置为0表示有多少个处理器就允许同时运行多少个线程
                                //如果ExistingCompletionPort不为0，系统忽略NumberOfConcurrentThreads

    if (m_hIOCompletionPort == NULL)
    {
        QingLog::Write(LL_ERROR, "Create IO completion port error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write("Created IO completion port succeed.", LL_INFO);
    return true;
}

QING_NAMESPACE_END
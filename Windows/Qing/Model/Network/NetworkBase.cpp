#include "NetworkBase.h"
#include "..\..\HeaderFiles\QingLog.h"
#include "..\..\HeaderFiles\LocalComputer.h"
#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN



NetworkBase::NetworkBase() : m_ServerListenPort(0),
                             m_hIOCompletionPort(NULL),
                             m_hWorkerThreadExitEvent(NULL)
{
    memset(m_WorkerThreads, NULL, sizeof(m_WorkerThreads));
}

NetworkBase::~NetworkBase()
{
    Stop();
}

bool NetworkBase::Start(const std::string & ServerIP, int Port)
{
    if (m_hWorkerThreadExitEvent != NULL)
    {
        QingLog::Write("Start succeed, repeat start.", LL_INFO);
        return false;
    }

    m_ServerIP = ServerIP;
    m_ServerListenPort = Port;
    m_hWorkerThreadExitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    return true;
}

void NetworkBase::Stop()
{
    if (m_WorkerThreads[0] != NULL)
    {
        //激活关闭消息通知
        SetEvent(m_hWorkerThreadExitEvent);

        //通知所有的完成端口操作退出
        for (auto Index = 0; Index < m_ThreadParamVector.size(); Index++)
        {
            PostQueuedCompletionStatus(m_hIOCompletionPort, 0, NULL, NULL);
        }

        //等待所有工作线程资源退出
        WaitForMultipleObjects(static_cast<DWORD>(m_ThreadParamVector.size()), m_WorkerThreads, TRUE, INFINITE);
    }

    //释放其它资源
    ReleaseHandle(m_hIOCompletionPort);
    ReleaseHandle(m_hWorkerThreadExitEvent);

    for (int Index = 0; Index < static_cast<int>(m_ThreadParamVector.size()); Index++)
    {
        if (m_WorkerThreads[Index] != NULL)
        {
            ReleaseHandle(m_WorkerThreads[Index]);
        }
    }
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

bool NetworkBase::CreateWorkerThread(int WorkerThreadCount)
{
    if (WorkerThreadCount <= 0)
    {
        LocalComputer MyComputer;
        WorkerThreadCount = WORKER_THREADS_PER_PROCESSOR * MyComputer.GetProcessorsCount();
    }

    if (WorkerThreadCount > MAX_WORKER_THREAD_COUNT)
    {
        WorkerThreadCount = MAX_WORKER_THREAD_COUNT;
    }

    for (int Count = 0; Count < WorkerThreadCount; Count++)
    {
        ServerWorkerThreadParam NewParam;
        NewParam.m_ThreadID = 0;
        NewParam.m_ThreadIndex = Count;
        NewParam.m_QingServer = this;
        m_ThreadParamVector.push_back(NewParam);
    }

    DWORD nThreadID;
    for (auto ThreadIndex = 0; ThreadIndex < m_ThreadParamVector.size(); ThreadIndex++)
    {
        m_WorkerThreads[ThreadIndex] = ::CreateThread(0, 0, CallBack_WorkerThread, (void*)(&m_ThreadParamVector[ThreadIndex]), 0, &nThreadID);

        m_ThreadParamVector[ThreadIndex].m_ThreadID = nThreadID;
        QingLog::Write(LL_INFO, "Created worker thread, Index = %d, DEC_ID = %d, HEX_ID = %x.",
            m_ThreadParamVector[ThreadIndex].m_ThreadIndex,
            m_ThreadParamVector[ThreadIndex].m_ThreadID,
            m_ThreadParamVector[ThreadIndex].m_ThreadID);
    }

    QingLog::Write(LL_INFO, "Created total = %d worker threads.", m_ThreadParamVector.size());
    return true;
}

DWORD NetworkBase::CallBack_WorkerThread(LPVOID lpParam)
{
    ServerWorkerThreadParam *pParam = (ServerWorkerThreadParam*)lpParam;
    NetworkBase *pNetwork = (NetworkBase*)pParam->m_QingServer;
    unsigned long ThreadID = (unsigned long)pParam->m_ThreadID;

    QingLog::Write(LL_INFO, "Worker Thread DEC_ID = %d, HEX_ID = %x start.", ThreadID, ThreadID);
    pNetwork->WorkerThread();
    QingLog::Write(LL_INFO, "Worker Thread DEC_ID = %d, HEX_ID = %x exit.", ThreadID, ThreadID);

    return 0;
}

QING_NAMESPACE_END
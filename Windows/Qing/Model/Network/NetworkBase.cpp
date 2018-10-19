#include "NetworkBase.h"
#include "NetworkEnvironment.h"
#include "..\..\HeaderFiles\BoostLog.h"
#include "..\..\Model\BoostFormat.h"
#include "..\..\HeaderFiles\LocalComputer.h"
#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN



NetworkBase::NetworkBase() : m_ServerPort(0),
                             m_hWorkerThreadExitEvent(NULL)
{
    HANDLE IOCPHandle = GlobalNetwork.GetIOCP();
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
        BoostLog::WriteInfo(L"Start succeed, repeat start.");
        return false;
    }

    m_hWorkerThreadExitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hWorkerThreadExitEvent == NULL)
    {
        BoostLog::WriteError(BoostFormat(L"Create WorkerThreadExitEvent error = %d.", GetLastError()));
        return false;
    }

    m_ServerIP = ServerIP;
    m_ServerPort = Port;
    return true;
}

void NetworkBase::Stop()
{
    if (m_hWorkerThreadExitEvent != NULL)
    {
        //����ر���Ϣ֪ͨ
        SetEvent(m_hWorkerThreadExitEvent);

        //֪ͨ���е���ɶ˿ڲ����˳�
        for (std::vector<WorkerThreadParam>::size_type Index = 0; Index < m_ThreadParamVector.size(); Index++)
        {
            PostQueuedCompletionStatus(GlobalNetwork.GetIOCP(), 0, NULL, NULL);
        }

        //�ȴ����й����߳���Դ�˳�
        WaitForMultipleObjects(static_cast<DWORD>(m_ThreadParamVector.size()), m_WorkerThreads, TRUE, INFINITE);

        //�ͷ�������Դ
        ReleaseHandle(m_hWorkerThreadExitEvent);

        for (int Index = 0; Index < static_cast<int>(m_ThreadParamVector.size()); Index++)
        {
            if (m_WorkerThreads[Index] != NULL)
            {
                ReleaseHandle(m_WorkerThreads[Index]);
            }
        }
    }
}

bool NetworkBase::IsRunning() const
{
    return m_hWorkerThreadExitEvent != NULL && !m_ThreadParamVector.empty();
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
    ServerAddress.sin_port = htons(m_ServerPort);

    if (m_ServerIP.empty())
    {
        ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        BoostLog::WriteInfo(BoostFormat(L"Fill address set IP = %s, Port = %d.", "INADDR_ANY", m_ServerPort));
    }
    else
    {
        inet_pton(AF_INET, m_ServerIP.c_str(), &(ServerAddress.sin_addr.s_addr));
        BoostLog::WriteInfo(BoostFormat(L"Fill address set IP = %s, Port = %d.", m_ServerIP.c_str(), m_ServerPort));
    }
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
        WorkerThreadParam NewParam;
        NewParam.m_ThreadID = 0;
        NewParam.m_ThreadIndex = Count;
        NewParam.m_Network = this;
        m_ThreadParamVector.push_back(NewParam);
    }

    DWORD nThreadID;
    for (std::vector<WorkerThreadParam>::size_type ThreadIndex = 0; ThreadIndex < m_ThreadParamVector.size(); ThreadIndex++)
    {
        m_WorkerThreads[ThreadIndex] = ::CreateThread(0, 0, CallBack_WorkerThread, (void*)(&m_ThreadParamVector[ThreadIndex]), 0, &nThreadID);

        m_ThreadParamVector[ThreadIndex].m_ThreadID = nThreadID;
        BoostLog::WriteInfo(BoostFormat(L"Created worker thread, Index = %d, DEC_ID = %d, HEX_ID = %x.",
            m_ThreadParamVector[ThreadIndex].m_ThreadIndex,
            m_ThreadParamVector[ThreadIndex].m_ThreadID,
            m_ThreadParamVector[ThreadIndex].m_ThreadID));
    }

    BoostLog::WriteInfo(BoostFormat(L"Created total = %d worker threads.", m_ThreadParamVector.size()));
    return true;
}

bool NetworkBase::PostRecv(IOCPContext &RecvIOCPContext)
{
    RecvIOCPContext.ResetBuffer();
    RecvIOCPContext.m_ActionType = IOCP_AT_RECV;

    //Ͷ��WSARecv����
    DWORD dwFlags = 0, dwBytes = 0;
    int nBytesRecv = WSARecv(
        RecvIOCPContext.m_AcceptSocket,   //Ͷ�����������socket
        &RecvIOCPContext.m_WSABuffer,     //���ջ�������WSABUF�ṹ���ɵ�����
        1,                                //������WSABUF�ṹ����������Ϊ1
        &dwBytes,                         //���յ����ֽ���
        &dwFlags,                         //ָ���־λ��ָ��
        &RecvIOCPContext.m_Overlapped,    //���socket��Ӧ���ص��ṹ
        NULL);                            //�������ֻ�����������ģʽ�вŻ��õ�

    //������ش��󣬲��Ҵ���Ĵ��벻��Pending��˵������ʧ��
    if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
    {
        BoostLog::WriteError(BoostFormat(L"Post recv failed, Socket = %I64d, IOCPContextID = %I64d, error = %d.",
            RecvIOCPContext.m_AcceptSocket, RecvIOCPContext.m_ContextID, WSAGetLastError()));
        return false;
    }

    BoostLog::WriteInfo(BoostFormat(L"Post recv succeed, Socket = %I64d, IOCPContextID = %I64d.",
        RecvIOCPContext.m_AcceptSocket, RecvIOCPContext.m_ContextID));
    return true;
}

bool NetworkBase::PostSend(IOCPContext &SendIOCPContext)
{
    SendIOCPContext.m_ActionType = IOCP_AT_SEND;

    //Ͷ��WSASend����
    DWORD dwFlags = 0, dwBytes = 0;
    int nBytesRecv = WSASend(
        SendIOCPContext.m_AcceptSocket,   //Ͷ�����������socket
        &SendIOCPContext.m_WSABuffer,     //���ջ�������WSABUF�ṹ���ɵ�����
        1,                                //������WSABUF�ṹ����������Ϊ1
        &dwBytes,                         //���͵��ֽ���
        dwFlags,                          //��־λ
        &SendIOCPContext.m_Overlapped,    //���socket��Ӧ���ص��ṹ
        NULL);                            //�������ֻ�����������ģʽ�вŻ��õ�

    //������ش��󣬲��Ҵ���Ĵ��벻��Pending��˵������ʧ��
    if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
    {
        BoostLog::WriteError(BoostFormat(L"Post send failed, Socket = %I64d, IOCPContextID = %I64d, error = %d.",
            SendIOCPContext.m_AcceptSocket, SendIOCPContext.m_ContextID, WSAGetLastError()));
        return false;
    }

    BoostLog::WriteInfo(BoostFormat(L"Post send succeed, Socket = %I64d, IOCPContextID = %I64d.",
        SendIOCPContext.m_AcceptSocket, SendIOCPContext.m_ContextID));
    return true;
}

DWORD NetworkBase::CallBack_WorkerThread(LPVOID lpParam)
{
    WorkerThreadParam *pParam = (WorkerThreadParam*)lpParam;
    NetworkBase *pNetwork = (NetworkBase*)pParam->m_Network;
    unsigned long ThreadID = (unsigned long)pParam->m_ThreadID;

    BoostLog::WriteInfo(BoostFormat(L"Worker Thread DEC_ID = %d, HEX_ID = %x start.", ThreadID, ThreadID));
    pNetwork->WorkerThread();
    BoostLog::WriteInfo(BoostFormat(L"Worker Thread DEC_ID = %d, HEX_ID = %x exit.", ThreadID, ThreadID));

    return 0;
}

QING_NAMESPACE_END
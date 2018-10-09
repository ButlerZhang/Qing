#include "QingClient.h"
#include "..\..\HeaderFiles\QingLog.h"
#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN



QingClient::QingClient() : m_ListenPort(0),
                           m_hConnectThread(NULL),
                           m_hShutdownEvent(NULL),
                           m_phWorkerThreads(NULL)
{
}

QingClient::~QingClient()
{
    Stop();
}

bool QingClient::Start(const std::string &ServerIP, int ServerPort)
{
    if (m_hShutdownEvent != NULL)
    {
        return true;
    }

    m_ServerIP = ServerIP;
    m_ListenPort = ServerPort;
    m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    DWORD nThreadID;
    m_hConnectThread = ::CreateThread(0, 0, CallBack_ConnectThread, (void *)this, 0, &nThreadID);

    QingLog::Write(LL_INFO, "Created connect thread, DEC_ID = %d, HEX_ID = %x.", nThreadID, nThreadID);
    return true;
}

void QingClient::Stop()
{
    if (m_hShutdownEvent != NULL)
    {
        SetEvent(m_hShutdownEvent);
        WaitForSingleObject(m_hConnectThread, INFINITE);

        for (auto Index = 0; Index < m_ThreadParamVector.size(); Index++)
        {
            closesocket(m_ThreadParamVector[Index].m_Socket);
        }

        WaitForMultipleObjects((DWORD)m_ThreadParamVector.size(), m_phWorkerThreads, TRUE, INFINITE);

        CleanUp();
    }
}

const std::string & QingClient::GetLocalIP()
{
    if (m_ServerIP.empty())
    {
        std::vector<std::string> IPVector;
        if (m_LocalComputer.GetIPAddress(IPVector))
        {
            m_ServerIP = IPVector[0];
        }
    }

    return m_ServerIP;
}

void QingClient::CleanUp()
{
    ReleaseHandle(m_hShutdownEvent);
    ReleaseHandle(m_hConnectThread);

    for (int Index = 0; Index < static_cast<int>(m_ThreadParamVector.size()); Index++)
    {
        ReleaseHandle(m_phWorkerThreads[Index]);
    }
}

void QingClient::ReleaseHandle(HANDLE & Handle)
{
    if (Handle != NULL && Handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(Handle);
        Handle = NULL;
    }
}

bool QingClient::EstablishConnections(int ThreadCount)
{
    DWORD nThreadID;
    m_phWorkerThreads = new HANDLE[ThreadCount];

    for (int Count = 0; Count < ThreadCount; Count++)
    {
        ClientWorkerThreadParam NewParam;
        NewParam.m_ThreadID = 0;
        NewParam.m_ThreadIndex = 0;
        NewParam.m_QingClient = this;
        NewParam.m_Socket = INVALID_SOCKET;
        memset(NewParam.m_Buffer, 0, sizeof(NewParam.m_Buffer));
        m_ThreadParamVector.push_back(NewParam);
    }

    for (int Count = 0; Count < ThreadCount; Count++)
    {
        if (WaitForSingleObject(m_hShutdownEvent, 0) == WAIT_OBJECT_0)
        {
            QingLog::Write("Stop connecte.", LL_INFO);
            return true;
        }

        if (!ConnectServer(&m_ThreadParamVector[Count].m_Socket, m_ServerIP, m_ListenPort))
        {
            QingLog::Write("Connect server failed.", LL_ERROR);
            CleanUp();
            return false;
        }

        m_ThreadParamVector[Count].m_ThreadIndex = Count;
        sprintf_s(m_ThreadParamVector[Count].m_Buffer, "Thread index = %d send message = %s", Count, "Hello World");

        Sleep(10);

        m_phWorkerThreads[Count] = CreateThread(0, 0, CallBack_WorkerThread, (void*)(&m_ThreadParamVector[Count]), 0, &nThreadID);
        m_ThreadParamVector[Count].m_ThreadID = nThreadID;

        QingLog::Write(LL_INFO, "Created worker thread, Index = %d, DEC_ID = %d, HEX_ID = %x.",
            m_ThreadParamVector[Count].m_ThreadIndex,
            m_ThreadParamVector[Count].m_ThreadID,
            m_ThreadParamVector[Count].m_ThreadID);
    }

    return true;
}

bool QingClient::ConnectServer(SOCKET * pSocket, std::string ServerIP, int nPort)
{
    *pSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*pSocket == INVALID_SOCKET)
    {
        QingLog::Write(LL_ERROR, "Create socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    struct sockaddr_in ServerAddress;
    ZeroMemory((char*)&ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(m_ListenPort);
    inet_pton(AF_INET, m_ServerIP.c_str(), &(ServerAddress.sin_addr.s_addr));

    if (connect(*pSocket, reinterpret_cast<const struct sockaddr*>(&ServerAddress), sizeof(ServerAddress)) == SOCKET_ERROR)
    {
        QingLog::Write(LL_ERROR, "Connect server failed, error = %d.", WSAGetLastError());
        closesocket(*pSocket);
        return false;
    }

    return true;
}

DWORD QingClient::CallBack_ConnectThread(LPVOID lpParam)
{
    QingClient *pQingClient = (QingClient*)lpParam;

    QingLog::Write("Connet thread start...");
    pQingClient->EstablishConnections(100);
    QingLog::Write("Connet thread stop...");

    return 0;
}

DWORD QingClient::CallBack_WorkerThread(LPVOID lpParam)
{
    ClientWorkerThreadParam *pParams = (ClientWorkerThreadParam*)lpParam;
    QingClient *pQingClient = (QingClient*)pParams->m_QingClient;

    char szTemp[MAX_IO_CONTEXT_BUFFER_LEN];
    memset(szTemp, 0, sizeof(szTemp));
    int nBytesSent = 0;

    //first
    sprintf_s(szTemp, ("First message£º%s"), pParams->m_Buffer);
    nBytesSent = send(pParams->m_Socket, szTemp, (int)strlen(szTemp), 0);
    if (SOCKET_ERROR == nBytesSent)
    {
        QingLog::Write(LL_ERROR, "Send first message failed, error = %d.", WSAGetLastError());
        return 1;
    }

    QingLog::Write(LL_INFO, "Send succeed, %s", szTemp);
    Sleep(3000);

    //second
    memset(szTemp, 0, sizeof(szTemp));
    sprintf_s(szTemp, ("Second message£º%s"), pParams->m_Buffer);
    nBytesSent = send(pParams->m_Socket, szTemp, (int)strlen(szTemp), 0);
    if (SOCKET_ERROR == nBytesSent)
    {
        QingLog::Write(LL_ERROR, "Send second message failed, error = %d.", WSAGetLastError());
        return 1;
    }

    QingLog::Write(LL_INFO, "Send succeed, %s", szTemp);
    Sleep(3000);

    //third
    memset(szTemp, 0, sizeof(szTemp));
    sprintf_s(szTemp, ("Third message£º%s"), pParams->m_Buffer);
    nBytesSent = send(pParams->m_Socket, szTemp, (int)strlen(szTemp), 0);
    if (SOCKET_ERROR == nBytesSent)
    {
        QingLog::Write(LL_ERROR, "Send third message failed, error = %d.", WSAGetLastError());
        return 1;
    }

    QingLog::Write(LL_INFO, "Send succeed, %s", szTemp);
    Sleep(3000);

    if (pParams->m_ThreadIndex == (static_cast<int>(pQingClient->m_ThreadParamVector.size()) - 1))
    {
        QingLog::Write(LL_INFO, "Test %d threads done.", static_cast<int>(pQingClient->m_ThreadParamVector.size()));
    }

    return 0;
}

QING_NAMESPACE_END
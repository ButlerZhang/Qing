#include "QingServer.h"
#include "..\..\HeaderFiles\QingLog.h"
#include "..\..\HeaderFiles\LocalComputer.h"
#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN



QingServer::QingServer() : m_hWorkerThreadExitEvent(NULL),
                           m_hIOCompletionPort(NULL),
                           m_ListenPort(8888),
                           m_ListenSocketContext(NULL),
                           m_CallBackAcceptEx(NULL),
                           m_CallBackGetAcceptExSockAddrs(NULL)
{
    memset(m_WorkerThreads, NULL, sizeof(m_WorkerThreads));
}

QingServer::~QingServer()
{
    Stop();
}

bool QingServer::Start(int ListenPort, const std::string &ServerIP)
{
    m_ServerBindIP = ServerIP;
    m_ListenPort = ListenPort;
    m_hWorkerThreadExitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

    if (!CreateIOCP() ||
        !CreateWorkerThread() ||
        !CreateAndStartListen() ||
        !InitializeAcceptExCallBack() ||
        !StartPostAcceptExIORequest())
    {
        return false;
    }

    QingLog::Write("Start succeed, QingServer ready.", Qing::LL_INFO);
    return true;
}

void QingServer::Stop()
{
    if (m_ListenSocketContext != NULL && m_ListenSocketContext->m_Socket != INVALID_SOCKET)
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

        QingLog::Write("Stop server.", Qing::LL_INFO);
    }
}

const std::string& QingServer::GetLocalIP()
{
    //char HostName[MAX_PATH];
    //memset(HostName, 0, MAX_PATH);

    ////获取主机名
    //gethostname(HostName, MAX_PATH);
    //hostent FAR* lpHostEnt = gethostbyname(HostName);
    //if (lpHostEnt == NULL)
    //{
    //    return m_ServerIP;
    //}

    ////获取IP地址列表中的第一个IP
    //LPSTR lpAddr = lpHostEnt->h_addr_list[0];

    ////将IP地址转换成字符串形式
    //struct in_addr inAddr;
    //memmove(&inAddr, lpAddr, 4);
    //m_ServerIP = inet_ntoa(inAddr);

    return m_ServerBindIP;
}

bool QingServer::CreateIOCP()
{
    m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (m_hIOCompletionPort == NULL)
    {
        QingLog::Write(Qing::LL_ERROR, "Create IO completion port error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write("Created IO completion port succeed.", LL_INFO);
    return true;
}

bool QingServer::CreateWorkerThread()
{
    Qing::LocalComputer MyComputer;
    int WorkerThreadCount = WORKER_THREADS_PER_PROCESSOR * MyComputer.GetProcessorsCount();
    if (WorkerThreadCount > MAX_WORKER_THREAD_COUNT)
    {
        WorkerThreadCount = MAX_WORKER_THREAD_COUNT;
    }

    for (int Count = 0; Count < WorkerThreadCount; Count++)
    {
        WorkerThreadParam NewParam;
        NewParam.m_ThreadID = Count;
        NewParam.m_QingServer = this;
        m_ThreadParamVector.push_back(NewParam);
    }

    DWORD nThreadID;
    for (int Count = 0; Count < WorkerThreadCount; Count++)
    {
        m_WorkerThreads[Count] = ::CreateThread(0, 0, WorkerThread, (void*)(&m_ThreadParamVector[Count]), 0, &nThreadID);
    }

    QingLog::Write(Qing::LL_INFO, "Created %d worker threads.", WorkerThreadCount);
    return true;
}

bool QingServer::CreateAndStartListen()
{
    m_ListenSocketContext = std::make_shared<IOCPSocketContext>();

    m_ListenSocketContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_ListenSocketContext->m_Socket == INVALID_SOCKET)
    {
        QingLog::Write(Qing::LL_ERROR, "Initialize listen socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write("Initialize listen socket succeed.", Qing::LL_INFO);

    //将listen socket绑定到完成端口
    if (CreateIoCompletionPort((HANDLE)m_ListenSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)(m_ListenSocketContext.get()), 0) == NULL)
    {
        QingLog::Write(Qing::LL_ERROR, "Bind listen socket to IOCP failed, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write("Bind listen socket to IOCP succeed.", Qing::LL_INFO);

    //填充地址信息
    struct sockaddr_in ServerAddress;
    ZeroMemory((char*)&ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(m_ListenPort);

    if (m_ServerBindIP.empty())
    {
        //绑定任何一个地址
        ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        //绑定某个IP地址
        inet_pton(AF_INET, m_ServerBindIP.c_str(), &(ServerAddress.sin_addr.s_addr));
    }

    //绑定地址和端口
    if (bind(m_ListenSocketContext->m_Socket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
    {
        QingLog::Write(Qing::LL_ERROR, "Listen socket bind failed, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write("Listen socket bind succeed.", Qing::LL_INFO);

    //开始进行监听
    if (listen(m_ListenSocketContext->m_Socket, SOMAXCONN) == SOCKET_ERROR)
    {
        QingLog::Write(Qing::LL_ERROR, "Listen error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write("Listening...", Qing::LL_INFO);
    return true;
}

bool QingServer::InitializeAcceptExCallBack()
{
    //使用AcceptEx函数，因为这个是WinSock2规范之外的扩展函数，
    //所以需要额外获取一下函数的指针
    DWORD dwBytes = 0;
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    if (SOCKET_ERROR == WSAIoctl(
        m_ListenSocketContext->m_Socket,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidAcceptEx,
        sizeof(GuidAcceptEx),
        &m_CallBackAcceptEx,
        sizeof(m_CallBackAcceptEx),
        &dwBytes,
        NULL,
        NULL))
    {
        QingLog::Write(Qing::LL_ERROR, "WSAIoctl can not get AcceptEx pointer, error = %d.", WSAGetLastError());
        return false;
    }

    //获取GetAcceptExSockAddrs函数指针
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    if (SOCKET_ERROR == WSAIoctl(
        m_ListenSocketContext->m_Socket,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidGetAcceptExSockAddrs,
        sizeof(GuidGetAcceptExSockAddrs),
        &m_CallBackGetAcceptExSockAddrs,
        sizeof(m_CallBackGetAcceptExSockAddrs),
        &dwBytes,
        NULL,
        NULL))
    {
        QingLog::Write(Qing::LL_ERROR, "WSAIoctl can not get GetAcceptExSockAddrs pointer, error = %d.", WSAGetLastError());
        return false;
    }

    return true;
}

bool QingServer::StartPostAcceptExIORequest()
{
    //为AcceptEx准备参数，然后投递AcceptEx IO请求
    for (int i = 0; i < MAX_POST_ACCEPT; i++)
    {
        std::shared_ptr<IOCPContext> pAcceptIOCPContext = m_ListenSocketContext->GetNewIOContext();
        if (!PostAccept(pAcceptIOCPContext.get()))
        {
            m_ListenSocketContext->RemoveContext(pAcceptIOCPContext);
            return false;
        }
    }

    QingLog::Write(Qing::LL_INFO, "Post %d AcceptEx request.", MAX_POST_ACCEPT);
    return true;
}

bool QingServer::IsSocketAlive(SOCKET socket)
{
    //判断客户端socket是否已经断开，否则在一个无效socket上投递WSARecv操作会抛出异常
    //判断方法是尝试向这个socket发送数据，判断这个socket调用的返回值
    //因为如果客户端网络异常断开(崩溃或拔掉网线)时服务端无法收到客户端断开的通知

    int nByteSend = send(socket, "", 0, 0);
    return (nByteSend != -1);
}

void QingServer::ReleaseHandle(HANDLE &Handle)
{
    if (Handle != NULL && Handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(Handle);
        Handle = NULL;
    }
}

bool QingServer::HandleError(std::shared_ptr<IOCPSocketContext> pSocketContext, DWORD ErrorCode)
{
    if (WAIT_TIMEOUT == ErrorCode)                          //超时
    {
        if (IsSocketAlive(pSocketContext->m_Socket))
        {
            QingLog::Write("Network time out, reconnecting....", Qing::LL_INFO);
            return true;
        }
        else
        {
            QingLog::Write("Client disconnected.", Qing::LL_INFO);
            //RemoveContextList(pSocketContext);
            return true;
        }
    }
    else if (ERROR_NETNAME_DELETED == ErrorCode)            //异常
    {
        QingLog::Write("Client disconnected.", Qing::LL_INFO);
        //RemoveContextList(pSocketContext);
        return true;
    }
    else
    {
        QingLog::Write(Qing::LL_ERROR, "IOCP error = %d, exit thread.", ErrorCode);
        return false;
    }
}

std::string QingServer::ConvertToIPString(SOCKADDR_IN *ClientAddr)
{
    char TempIPArray[INET_ADDRSTRLEN];
    memset(TempIPArray, 0, sizeof(TempIPArray));

    inet_ntop(AF_INET, &(ClientAddr->sin_addr), TempIPArray, sizeof(TempIPArray));
    return std::string(TempIPArray);
}

bool QingServer::PostAccept(IOCPContext *pIOCPContext)
{
    //为新连入的客户端先准备好socket(这是与传统accept最大的区别)
    pIOCPContext->m_AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (pIOCPContext->m_AcceptSocket == INVALID_SOCKET)
    {
        QingLog::Write(Qing::LL_ERROR, "Create new accept socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    DWORD dwBytes = 0;
    pIOCPContext->m_ActionType = IOCP_AT_ACCEPT;

    //投递AcceptEx
    if (!m_CallBackAcceptEx(
        m_ListenSocketContext->m_Socket,
        pIOCPContext->m_AcceptSocket,
        pIOCPContext->m_WSABuffer.buf,
        pIOCPContext->m_WSABuffer.len - (ACCEPTEX_ADDRESS_LENGTH * 2),
        ACCEPTEX_ADDRESS_LENGTH,
        ACCEPTEX_ADDRESS_LENGTH,
        &dwBytes,
        &pIOCPContext->m_Overlapped))
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            QingLog::Write(Qing::LL_ERROR, "Post accept failed, error = %d.", WSAGetLastError());
            return false;
        }
    }

    return true;
}

bool QingServer::PostRecv(IOCPContext *pIOCPContext)
{
    pIOCPContext->ResetBuffer();
    pIOCPContext->m_ActionType = IOCP_AT_RECV;

    //投递WSARecv请求
    DWORD dwFlags = 0, dwBytes = 0;
    int nBytesRecv = WSARecv(pIOCPContext->m_AcceptSocket, &pIOCPContext->m_WSABuffer, 1, &dwBytes, &dwFlags, &pIOCPContext->m_Overlapped, NULL);

    //如果返回错误，并且错误的代码不是Pending，说明请求失败
    if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
    {
        QingLog::Write(Qing::LL_ERROR, "Post recv failed, error = %d.", WSAGetLastError());
        return false;
    }

    return true;
}

bool QingServer::PostSend(IOCPContext *pIOCPContext)
{
    QingLog::Write("Post send succeed.");
    return true;
}

bool QingServer::DoAccept(IOCPSocketContext * pSocketContext, IOCPContext *pIOCPContext)
{
    SOCKADDR_IN *ClientAddr = NULL;
    SOCKADDR_IN *LocalAddr = NULL;
    int RemoteLen = sizeof(SOCKADDR_IN);
    int LocalLen = sizeof(SOCKADDR_IN);

    //1.获取连入客户端的地址信息
    m_CallBackGetAcceptExSockAddrs(
        pIOCPContext->m_WSABuffer.buf,
        pIOCPContext->m_WSABuffer.len - (ACCEPTEX_ADDRESS_LENGTH * 2),
        ACCEPTEX_ADDRESS_LENGTH,
        ACCEPTEX_ADDRESS_LENGTH,
        (LPSOCKADDR*)&LocalAddr,
        &LocalLen,
        (LPSOCKADDR*)&ClientAddr,
        &RemoteLen);

    QingLog::Write(Qing::LL_INFO, "Client %s:%d connected, message = %s.",
        ConvertToIPString(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_WSABuffer.buf);

    //2.拷贝Listen socket上的Context，为新连入的socket创建一个新的SocketContext
    std::shared_ptr<IOCPSocketContext> pNewIOCPSocketContext = std::make_shared<IOCPSocketContext>();
    pNewIOCPSocketContext->m_Socket = pIOCPContext->m_AcceptSocket;
    memcpy(&(pNewIOCPSocketContext->m_ClientAddr), ClientAddr, sizeof(SOCKADDR_IN));

    //将这个新的Socket和完成端口绑定
    if (CreateIoCompletionPort((HANDLE)pNewIOCPSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)(pNewIOCPSocketContext.get()), 0) == NULL)
    {
        QingLog::Write(Qing::LL_ERROR, "Associate with IOCP error = %d.", GetLastError());
        return false;
    }

    //3.建立新Socket下的IOContext，用于在这个socket上投递第一个Recv数据请求
    std::shared_ptr<IOCPContext> pNewIOCPContext = pNewIOCPSocketContext->GetNewIOContext();
    pNewIOCPContext->m_ActionType = IOCP_AT_RECV;
    pNewIOCPContext->m_AcceptSocket = pNewIOCPSocketContext->m_Socket;
    //如果Buffer需要保留，就拷贝出来
    //memcpy(pNewIOCPContext->m_szBuffer, pIOCPContext->m_szBuffer, MAX_BUFFER_LEN);

    //绑定完成之后，就可以开始在这个socket上投递完成请求了
    if (!PostRecv(pNewIOCPContext.get()))
    {
        pNewIOCPSocketContext->RemoveContext(pNewIOCPContext);
        return false;
    }

    //4.如果投递成功，则记录这个有效的客户端信息
    m_ClientManager.AddClient(pNewIOCPSocketContext);

    //5.重置listen socket，准备投递新的AcceptEx
    pIOCPContext->ResetBuffer();
    return PostAccept(pIOCPContext);
}

bool QingServer::DoRecv(IOCPSocketContext * pSocketContext, IOCPContext *pIOCPContext)
{
    //处理消息
    SOCKADDR_IN *ClientAddr = &pSocketContext->m_ClientAddr;
    QingLog::Write(Qing::LL_INFO, "Recv %s:%d message = %s",
        ConvertToIPString(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_WSABuffer.buf);

    //开始投递下一个WSARecv请求
    return PostRecv(pIOCPContext);
}

bool QingServer::DoSend(IOCPSocketContext * pSocketContext, IOCPContext *pIOCPContext)
{
    return false;
}

DWORD QingServer::WorkerThread(LPVOID lpParam)
{
    WorkerThreadParam *pParam = (WorkerThreadParam*)lpParam;
    QingServer *pQingIOCP = (QingServer*)pParam->m_QingServer;
    QingLog::Write(Qing::LL_INFO, "Worker Thread ID = %d start.", (int)pParam->m_ThreadID);

    OVERLAPPED *pOverlapped = NULL;
    DWORD dwBytesTransfered = 0;

    //循环处理，直到收到Shutdown消息为止
    while (WaitForSingleObject(pQingIOCP->m_hWorkerThreadExitEvent, 0) != WAIT_OBJECT_0)
    {
        std::shared_ptr<IOCPSocketContext> pSocketContext;

        BOOL bReturn = GetQueuedCompletionStatus(
            pQingIOCP->m_hIOCompletionPort,
            &dwBytesTransfered,
            (PULONG_PTR)&pSocketContext,
            &pOverlapped,
            INFINITE);

        //如果收到的是退出消息则直接退出
        if (pSocketContext == NULL)
        {
            break;
        }

        if (!bReturn)
        {
            if (!pQingIOCP->HandleError(pSocketContext, GetLastError()))
            {
                break;
            }

            continue;
        }
        else
        {
            //读取传入的参数
            IOCPContext *pIOCPContext(CONTAINING_RECORD(pOverlapped, IOCPContext, m_Overlapped));

            //判断是否有客户端断开了
            if ((0 == dwBytesTransfered) && (IOCP_AT_RECV == pIOCPContext->m_ActionType || IOCP_AT_SEND == pIOCPContext->m_ActionType))
            {
                QingLog::Write(Qing::LL_ERROR, "Client %s:%d disconnected.",
                    pQingIOCP->ConvertToIPString(&(pSocketContext->m_ClientAddr)).c_str(),
                    ntohs(pSocketContext->m_ClientAddr.sin_port));

                //释放资源
                pQingIOCP->m_ClientManager.RemoveClient(pSocketContext);
                continue;
            }
            else
            {
                switch (pIOCPContext->m_ActionType)
                {
                case IOCP_AT_ACCEPT:    pQingIOCP->DoAccept(pSocketContext.get(), pIOCPContext);        break;
                case IOCP_AT_RECV:      pQingIOCP->DoRecv(pSocketContext.get(), pIOCPContext);          break;
                case IOCP_AT_SEND:      pQingIOCP->DoSend(pSocketContext.get(), pIOCPContext);          break;
                default:                QingLog::Write("Worker thread action type error");              break;
                }
            }
        }
    }

    QingLog::Write(Qing::LL_INFO, "Worker Thread ID = %d exit.", (int)pParam->m_ThreadID);
    return 0;
}

QING_NAMESPACE_END
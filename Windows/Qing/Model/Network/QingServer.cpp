#include "QingServer.h"
#include "..\..\HeaderFiles\QingLog.h"
#include "..\..\HeaderFiles\LocalComputer.h"
#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN


struct WorkerThreadParam
{
    QingServer    *m_pQingIOCP;       //用于调用类中的函数
    int          m_nThreadNo;       //线程编号
};



QingServer::QingServer() : m_hShutdownEvent(NULL),
                           m_hIOCompletionPort(NULL),
                           m_phWorkerThreads(NULL),
                           m_WorkerThreadCount(0),
                           m_ListenPort(8888),
                           m_ListenSocketContext(NULL),
                           m_CallBackAcceptEx(NULL),
                           m_CallBackGetAcceptExSockAddrs(NULL)
{
}

QingServer::~QingServer()
{
    Stop();
}

bool QingServer::Start(int ListenPort, const std::string &ServerIP)
{
    m_ServerBindIP = ServerIP;
    m_ListenPort = ListenPort;

    //建立线程退出的事件通知
    m_hShutdownEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

    //初始化IOCP
    if (!InitializeIOCP())
    {
        Qing::QingLog::Write("Initialize IOCP failed.", Qing::LL_ERROR);
        return false;
    }

    Qing::QingLog::Write("Initialize IOCP succeed.", Qing::LL_INFO);

    //初始化Socket
    if (!InitializeListenSocket())
    {
        Qing::QingLog::Write("Initialize listen socket failed.", Qing::LL_ERROR);
        DeleteInitializeResources();
        return false;
    }

    Qing::QingLog::Write("Initialize listen socket succeed.", Qing::LL_INFO);
    Qing::QingLog::Write("QingServer ready.", Qing::LL_INFO);
    return true;
}

void QingServer::Stop()
{
    if (m_ListenSocketContext != NULL && m_ListenSocketContext->m_Socket != INVALID_SOCKET)
    {
        //激活关闭消息通知
        SetEvent(m_hShutdownEvent);

        //通知所有的完成端口操作退出
        for (int i = 0; i < m_WorkerThreadCount; i++)
        {
            PostQueuedCompletionStatus(m_hIOCompletionPort, 0, NULL, NULL);
        }

        //等待所有的客户端资源退出
        WaitForMultipleObjects(m_WorkerThreadCount, m_phWorkerThreads, TRUE, INFINITE);

        //释放其它资源
        DeleteInitializeResources();

        Qing::QingLog::Write("Stop listen.", Qing::LL_INFO);
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

bool QingServer::InitializeIOCP()
{
    //创建完成端口
    m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == m_hIOCompletionPort)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Create IO completion port error = %d.", WSAGetLastError());
        return false;
    }

    //根据处理器个数创建对应的线程数
    Qing::LocalComputer MyComputer;
    m_WorkerThreadCount = WORKER_THREADS_PER_PROCESSOR * MyComputer.GetProcessorsCount();
    m_phWorkerThreads = new HANDLE[m_WorkerThreadCount];

    //创建worker线程
    DWORD nThreadID;
    for (int i = 0; i < m_WorkerThreadCount; i++)
    {
        WorkerThreadParam *pWorkerThreadParam = new WorkerThreadParam();
        pWorkerThreadParam->m_pQingIOCP = this;
        pWorkerThreadParam->m_nThreadNo = i + 1;
        m_phWorkerThreads[i] = ::CreateThread(0, 0, WorkerThread, (void*)pWorkerThreadParam, 0, &nThreadID);
    }

    Qing::QingLog::Write(Qing::LL_INFO, "Created %d worker threads.", m_WorkerThreadCount);
    return true;
}

bool QingServer::InitializeListenSocket()
{
    //生成用于监听的socket信息
    m_ListenSocketContext = std::make_shared<IOCPSocketContext>();

    //使用WSASocket建立socket，才可以支持重叠IO操作
    m_ListenSocketContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_ListenSocketContext->m_Socket == INVALID_SOCKET)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Initialize listen socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Initialize listen socket succeed.", Qing::LL_INFO);

    //将listen socket绑定到完成端口
    if (CreateIoCompletionPort((HANDLE)m_ListenSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)(m_ListenSocketContext.get()), 0) == NULL)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Bind listen socket to IOCP failed, error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Bind listen socket to IOCP succeed.", Qing::LL_INFO);

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
        Qing::QingLog::Write(Qing::LL_ERROR, "Listen socket bind failed, error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Listen socket bind succeed.", Qing::LL_INFO);

    //开始进行监听
    if (listen(m_ListenSocketContext->m_Socket, SOMAXCONN) == SOCKET_ERROR)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Listen error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Listening...", Qing::LL_INFO);

    //用于导出函数指针
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;

    //使用AcceptEx函数，因为这个是WinSock2规范之外的扩展函数，
    //所以需要额外获取一下函数的指针
    DWORD dwBytes = 0;
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
        Qing::QingLog::Write(Qing::LL_ERROR, "WSAIoctl can not get AcceptEx pointer, error = %d.", WSAGetLastError());
        DeleteInitializeResources();
        return false;
    }

    //获取GetAcceptExSockAddrs函数指针
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
        Qing::QingLog::Write(Qing::LL_ERROR, "WSAIoctl can not get GetAcceptExSockAddrs pointer, error = %d.", WSAGetLastError());
        DeleteInitializeResources();
        return false;
    }

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

    Qing::QingLog::Write(Qing::LL_INFO, "Post %d AcceptEx request.", MAX_POST_ACCEPT);
    return true;
}

void QingServer::DeleteInitializeResources()
{
    ReleaseHandle(m_hShutdownEvent);

    for (int i = 0; i < m_WorkerThreadCount; i++)
    {
        ReleaseHandle(m_phWorkerThreads[i]);
    }

    delete m_phWorkerThreads;
    m_phWorkerThreads = NULL;

    ReleaseHandle(m_hIOCompletionPort);

    Qing::QingLog::Write("Delete resources succeed.", Qing::LL_INFO);
}

bool QingServer::PostAccept(IOCPContext *pIOCPContext)
{
    DWORD dwBytes = 0;
    pIOCPContext->m_ActionType = IOCP_AT_ACCEPT;
    WSABUF *pWSABUF = &pIOCPContext->m_WSABuffer;
    OVERLAPPED *pOverlapped = &pIOCPContext->m_Overlapped;

    //为新连入的客户端先准备好socket(这是与传统accept最大的区别)
    pIOCPContext->m_AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (pIOCPContext->m_AcceptSocket == INVALID_SOCKET)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Create accept socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    //投递AcceptEx
    if (!m_CallBackAcceptEx(
        m_ListenSocketContext->m_Socket,
        pIOCPContext->m_AcceptSocket,
        pWSABUF->buf,
        pWSABUF->len - ((sizeof(SOCKADDR_IN) + 16) * 2),
        sizeof(SOCKADDR_IN) + 16,
        sizeof(SOCKADDR_IN) + 16,
        &dwBytes,
        pOverlapped))
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            Qing::QingLog::Write(Qing::LL_ERROR, "Post accept failed, error = %d.", WSAGetLastError());
            return false;
        }
    }

    return true;
}

bool QingServer::PostRecv(IOCPContext *pIOCPContext)
{
    DWORD dwFlags = 0;
    DWORD dwBytes = 0;
    WSABUF *pWSABUF = &pIOCPContext->m_WSABuffer;
    OVERLAPPED *pOverlapped = &pIOCPContext->m_Overlapped;

    pIOCPContext->ResetBuffer();
    pIOCPContext->m_ActionType = IOCP_AT_RECV;

    //投递WSARecv请求
    int nBytesRecv = WSARecv(pIOCPContext->m_AcceptSocket, pWSABUF, 1, &dwBytes, &dwFlags, pOverlapped, NULL);

    //如果返回错误，并且错误的代码不是Pending，说明请求失败
    if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Post recv failed, error = %d.", WSAGetLastError());
        return false;
    }

    return true;
}

bool QingServer::PostSend(IOCPContext *pIOCPContext)
{
    Qing::QingLog::Write("Post send succeed.");
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
        pIOCPContext->m_WSABuffer.len - ((sizeof(SOCKADDR_IN) + 16) * 2),
        sizeof(SOCKADDR_IN) + 16,
        sizeof(SOCKADDR_IN) + 16,
        (LPSOCKADDR*)&LocalAddr,
        &LocalLen,
        (LPSOCKADDR*)&ClientAddr,
        &RemoteLen);

    Qing::QingLog::Write(Qing::LL_INFO, "Client %s:%d connected, message = %s.",
        ClientIP(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_WSABuffer.buf);

    //2.拷贝Listen socket上的Context，为新连入的socket创建一个新的SocketContext
    std::shared_ptr<IOCPSocketContext> pNewIOCPSocketContext = std::make_shared<IOCPSocketContext>();
    pNewIOCPSocketContext->m_Socket = pIOCPContext->m_AcceptSocket;
    memcpy(&(pNewIOCPSocketContext->m_ClientAddr), ClientAddr, sizeof(SOCKADDR_IN));

    //将这个新的Socket和完成端口绑定
    if (!AssociateWithIOCP(pNewIOCPSocketContext.get()))
    {
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
    Qing::QingLog::Write(Qing::LL_INFO, "Recv %s:%d message = %s",
        ClientIP(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_WSABuffer.buf);

    //开始投递下一个WSARecv请求
    return PostRecv(pIOCPContext);
}

bool QingServer::DoSend(IOCPSocketContext * pSocketContext, IOCPContext *pIOCPContext)
{
    return false;
}

bool QingServer::IsSocketAlive(SOCKET socket)
{
    //判断客户端socket是否已经断开，否则在一个无效socket上投递WSARecv操作会抛出异常
    //方法是尝试向这个socket发送数据，判断这个socket调用的返回值
    //因为如果客户端网络异常断开(崩溃或拔掉网线)时服务端无法收到客户端断开的通知

    int nByteSend = send(socket, "", 0, 0);
    return (nByteSend != -1);
}

void QingServer::ReleaseHandle(HANDLE Handle)
{
    if (Handle != NULL && Handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(Handle);
        Handle = NULL;
    }
}

bool QingServer::AssociateWithIOCP(IOCPSocketContext * pSocketContext)
{
    HANDLE Handle = CreateIoCompletionPort((HANDLE)pSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)pSocketContext, 0);
    if (Handle == NULL)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Associate with IOCP error = %d.", GetLastError());
        return false;
    }

    return true;
}

bool QingServer::HandleError(IOCPSocketContext * pSocketContext, const DWORD & dwErr)
{
    if (WAIT_TIMEOUT == dwErr)                          //超时
    {
        if (IsSocketAlive(pSocketContext->m_Socket))
        {
            Qing::QingLog::Write("Network time out, reconnecting....", Qing::LL_INFO);
            return true;
        }
        else
        {
            Qing::QingLog::Write("Client disconnected.", Qing::LL_INFO);
            //RemoveContextList(pSocketContext);
            return true;
        }
    }
    else if (ERROR_NETNAME_DELETED == dwErr)            //异常
    {
        Qing::QingLog::Write("Client disconnected.", Qing::LL_INFO);
        //RemoveContextList(pSocketContext);
        return true;
    }
    else
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "IOCP error = %d, exit thread.", dwErr);
        return false;
    }
}

std::string QingServer::ClientIP(SOCKADDR_IN *ClientAddr)
{
    char TempIPArray[INET_ADDRSTRLEN];
    memset(TempIPArray, 0, sizeof(TempIPArray));

    inet_ntop(AF_INET, &(ClientAddr->sin_addr), TempIPArray, sizeof(TempIPArray));
    return std::string(TempIPArray);
}

DWORD QingServer::WorkerThread(LPVOID lpParam)
{
    WorkerThreadParam *pParam = (WorkerThreadParam*)lpParam;
    QingServer *pQingIOCP = (QingServer*)pParam->m_pQingIOCP;
    int nThreadNo = (int)pParam->m_nThreadNo;
    Qing::QingLog::Write(Qing::LL_INFO, "Worker thread ID = %d start.", nThreadNo);

    OVERLAPPED *pOverlapped = NULL;
    //IOCPSocketContext *pSocketContext = NULL;
    DWORD dwBytesTransfered = 0;

    //循环处理，直到收到Shutdown消息为止
    while (WaitForSingleObject(pQingIOCP->m_hShutdownEvent, 0) != WAIT_OBJECT_0)
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
            DWORD dwErr = GetLastError();
            if (!pQingIOCP->HandleError(pSocketContext.get(), dwErr))
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
                Qing::QingLog::Write(Qing::LL_ERROR, "Client %s:%d disconnected.",
                    ClientIP(&(pSocketContext->m_ClientAddr)).c_str(),
                    ntohs(pSocketContext->m_ClientAddr.sin_port));

                //释放资源
                pQingIOCP->m_ClientManager.RemoveClient(pSocketContext);
                continue;
            }
            else
            {
                switch (pIOCPContext->m_ActionType)
                {
                case IOCP_AT_ACCEPT:    pQingIOCP->DoAccept(pSocketContext.get(), pIOCPContext);              break;
                case IOCP_AT_RECV:      pQingIOCP->DoRecv(pSocketContext.get(), pIOCPContext);                break;
                case IOCP_AT_SEND:      pQingIOCP->DoSend(pSocketContext.get(), pIOCPContext);                break;
                default:                Qing::QingLog::Write("Worker thread action type error");              break;
                }
            }
        }
    }

    Qing::QingLog::Write(Qing::LL_INFO, "Worker thread ID = %d exit.", nThreadNo);
    delete lpParam;
    lpParam = NULL;

    return 0;
}

QING_NAMESPACE_END
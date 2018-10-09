#include "NetworkServer.h"
#include "..\..\HeaderFiles\QingLog.h"
#include "..\..\HeaderFiles\LocalComputer.h"

QING_NAMESPACE_BEGIN



NetworkServer::NetworkServer() : m_hWorkerThreadExitEvent(NULL),
                           
                           m_ListenSocketContext(NULL),
                           m_CallBackAcceptEx(NULL),
                           m_CallBackGetAcceptExSockAddrs(NULL)
{
    memset(m_WorkerThreads, NULL, sizeof(m_WorkerThreads));
}

NetworkServer::~NetworkServer()
{
    Stop();
}

bool NetworkServer::Start(const std::string &ServerIP, int Port)
{
    if (m_hWorkerThreadExitEvent != NULL)
    {
        QingLog::Write("Start succeed, repeat start.", LL_INFO);
        return true;
    }

    NetworkBase::Start(ServerIP, Port);
    m_hWorkerThreadExitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

    if (!CreateIOCP() ||
        !CreateWorkerThread() ||
        !CreateAndStartListen() ||
        !InitializeAcceptExCallBack() ||
        !StartPostAcceptExIORequest())
    {
        return false;
    }

    QingLog::Write("Start succeed, NetworkServer ready.", LL_INFO);
    return true;
}

void NetworkServer::Stop()
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

        QingLog::Write("Stop server.", LL_INFO);
    }
}

bool NetworkServer::CreateWorkerThread()
{
    LocalComputer MyComputer;
    int WorkerThreadCount = WORKER_THREADS_PER_PROCESSOR * MyComputer.GetProcessorsCount();
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
    for (int Count = 0; Count < WorkerThreadCount; Count++)
    {
        m_WorkerThreads[Count] = ::CreateThread(0, 0, CallBack_WorkerThread, (void*)(&m_ThreadParamVector[Count]), 0, &nThreadID);

        m_ThreadParamVector[Count].m_ThreadID = nThreadID;
        QingLog::Write(LL_INFO, "Created worker thread, Index = %d, DEC_ID = %d, HEX_ID = %x.",
            m_ThreadParamVector[Count].m_ThreadIndex,
            m_ThreadParamVector[Count].m_ThreadID,
            m_ThreadParamVector[Count].m_ThreadID);
    }

    QingLog::Write(LL_INFO, "Created total = %d worker threads.", WorkerThreadCount);
    return true;
}

bool NetworkServer::CreateAndStartListen()
{
    m_ListenSocketContext = std::make_shared<IOCPSocketContext>();

    //想要使用重叠IO,初始化Socket时一定要使用WSASocket并带上WSA_FLAG_OVERLAPPED参数
    m_ListenSocketContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_ListenSocketContext->m_Socket == INVALID_SOCKET)
    {
        QingLog::Write(LL_ERROR, "Initialize listen socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write("Initialize listen socket succeed.", LL_INFO);

    //将listen socket绑定到完成端口
    //第三个参数CompletionKey类似于线程参数，在worker线程中就可以使用这个参数了
    if (CreateIoCompletionPort((HANDLE)m_ListenSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)(m_ListenSocketContext.get()), 0) == NULL)
    {
        QingLog::Write(LL_ERROR, "Bind listen socket to IOCP failed, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write("Bind listen socket to IOCP succeed.", LL_INFO);

    //绑定地址和端口
    struct sockaddr_in ServerAddress;
    FillAddress(ServerAddress);
    if (bind(m_ListenSocketContext->m_Socket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
    {
        QingLog::Write(LL_ERROR, "Listen socket bind failed, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write(LL_INFO, "Listen socket bind succeed, IP = %s, Port = %d.", GetLocalIP().c_str(), m_ServerListenPort);

    //开始进行监听
    //SOMAXCONN:Maximum queue length specifiable by listen
    if (listen(m_ListenSocketContext->m_Socket, SOMAXCONN) == SOCKET_ERROR)
    {
        QingLog::Write(LL_ERROR, "Listen error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write("Listening...", LL_INFO);
    return true;
}

bool NetworkServer::InitializeAcceptExCallBack()
{
    //AcceptEx不属于Winsock2体系，如果每次都直接调用它，Service Provider每次
    //都要通过WSAIoctl()获取该函数的指针，倒不如直接在代码中先获取一次它的指针。
    //GetAcceptExSockAddrs函数同理。

    //获取AcceptEx函数指针
    DWORD dwBytes = 0;
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    if (SOCKET_ERROR == WSAIoctl(
        m_ListenSocketContext->m_Socket,        //与socket无关，因为只是获取函数指针
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidAcceptEx,                          //已经定义好的GUID
        sizeof(GuidAcceptEx),
        &m_CallBackAcceptEx,
        sizeof(m_CallBackAcceptEx),
        &dwBytes,
        NULL,
        NULL))
    {
        QingLog::Write(LL_ERROR, "WSAIoctl can not get AcceptEx pointer, error = %d.", WSAGetLastError());
        return false;
    }

    //获取GetAcceptExSockAddrs函数指针
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    if (SOCKET_ERROR == WSAIoctl(
        m_ListenSocketContext->m_Socket,        //与socket无关，因为只是获取函数指针
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidGetAcceptExSockAddrs,              //已经定义好的GUID
        sizeof(GuidGetAcceptExSockAddrs),
        &m_CallBackGetAcceptExSockAddrs,
        sizeof(m_CallBackGetAcceptExSockAddrs),
        &dwBytes,
        NULL,
        NULL))
    {
        QingLog::Write(LL_ERROR, "WSAIoctl can not get GetAcceptExSockAddrs pointer, error = %d.", WSAGetLastError());
        return false;
    }

    return true;
}

bool NetworkServer::StartPostAcceptExIORequest()
{
    int InitAcceptPostCount = static_cast<int>(m_ThreadParamVector.size()) / 2;
    if (InitAcceptPostCount <= 0)
    {
        LocalComputer MyComputer;
        InitAcceptPostCount = MyComputer.GetProcessorsCount();
    }

    for (int i = 0; i < InitAcceptPostCount; i++)
    {
        std::shared_ptr<IOCPContext> pAcceptIOCPContext = m_ListenSocketContext->GetNewIOContext();
        if (!PostAccept(pAcceptIOCPContext.get()))
        {
            m_ListenSocketContext->RemoveContext(pAcceptIOCPContext);
            return false;
        }
    }

    QingLog::Write(LL_INFO, "Post %d AcceptEx request.", InitAcceptPostCount);
    return true;
}

bool NetworkServer::IsSocketAlive(SOCKET socket)
{
    //判断客户端socket是否已经断开，否则在一个无效socket上投递WSARecv操作会抛出异常
    //判断方法是尝试向这个socket发送数据，判断这个socket调用的返回值
    //因为如果客户端网络异常断开(崩溃或拔掉网线)时服务端无法收到客户端断开的通知

    int nByteSend = send(socket, "", 0, 0);
    return (nByteSend != -1);
}

bool NetworkServer::HandleError(const std::shared_ptr<IOCPSocketContext> &pSocketContext, DWORD ErrorCode)
{
    if (WAIT_TIMEOUT == ErrorCode)                          //超时
    {
        if (IsSocketAlive(pSocketContext->m_Socket))
        {
            QingLog::Write("Network time out, reconnecting....", LL_INFO);
            return true;
        }
        else
        {
            QingLog::Write("Client disconnected.", LL_INFO);
            m_ClientManager.RemoveClient(pSocketContext);
            return true;
        }
    }
    else if (ERROR_NETNAME_DELETED == ErrorCode)            //异常
    {
        QingLog::Write("Client disconnected.", LL_INFO);
        m_ClientManager.RemoveClient(pSocketContext);
        return true;
    }
    else
    {
        QingLog::Write(LL_ERROR, "IOCP error = %d, exit thread.", ErrorCode);
        return false;
    }
}

bool NetworkServer::PostAccept(IOCPContext *pIOCPContext)
{
    //为新连入的客户端先准备好socket(这是与传统accept最大的区别)
    pIOCPContext->m_AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (pIOCPContext->m_AcceptSocket == INVALID_SOCKET)
    {
        QingLog::Write(LL_ERROR, "Create new accept socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    DWORD dwBytes = 0;
    pIOCPContext->m_ActionType = IOCP_AT_ACCEPT;

    //投递AcceptEx
    if (!m_CallBackAcceptEx(
        m_ListenSocketContext->m_Socket,    //用来监听的socket
        pIOCPContext->m_AcceptSocket,       //用于接受连接的socket
        pIOCPContext->m_WSABuffer.buf,      //接收缓冲区：客户端发来的第一组数据；server的地址；client的地址
        pIOCPContext->m_WSABuffer.len - (ACCEPTEX_ADDRESS_LENGTH * 2),
        ACCEPTEX_ADDRESS_LENGTH,            //存放本地地址信息的空间大小
        ACCEPTEX_ADDRESS_LENGTH,            //存放远端地址信息的空间大小
        &dwBytes,                           //忽略
        &pIOCPContext->m_Overlapped))       //本次重叠IO所要用到的重叠结构
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            QingLog::Write(LL_ERROR, "Post accept failed, error = %d.", WSAGetLastError());
            return false;
        }
    }

    return true;
}

bool NetworkServer::PostRecv(IOCPContext *pIOCPContext)
{
    pIOCPContext->ResetBuffer();
    pIOCPContext->m_ActionType = IOCP_AT_RECV;

    //投递WSARecv请求
    DWORD dwFlags = 0, dwBytes = 0;
    int nBytesRecv = WSARecv(
        pIOCPContext->m_AcceptSocket,   //投递这个操作的socket
        &pIOCPContext->m_WSABuffer,     //接收缓冲区，WSABUF结构构成的数组
        1,                              //数组中WSABUF结构的数量，设为1
        &dwBytes,                       //接收到的字节数
        &dwFlags,                       //设置为0
        &pIOCPContext->m_Overlapped,    //这个socket对应的重叠结构
        NULL);                          //这个参数只有在完成例程模式中才会用到

    //如果返回错误，并且错误的代码不是Pending，说明请求失败
    if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
    {
        QingLog::Write(LL_ERROR, "Post recv failed, error = %d.", WSAGetLastError());
        return false;
    }

    return true;
}

bool NetworkServer::PostSend(IOCPContext *pIOCPContext)
{
    pIOCPContext->ResetBuffer();
    pIOCPContext->m_ActionType = IOCP_AT_SEND;

    //投递WSASend请求
    DWORD dwFlags = 0, dwBytes = 0;
    int nBytesRecv = WSASend(
        pIOCPContext->m_AcceptSocket,   //投递这个操作的socket
        &pIOCPContext->m_WSABuffer,     //接收缓冲区，WSABUF结构构成的数组
        1,                              //数组中WSABUF结构的数量，设为1
        &dwBytes,                       //接收到的字节数
        dwFlags,                        //设置为0
        &pIOCPContext->m_Overlapped,    //这个socket对应的重叠结构
        NULL);                          //这个参数只有在完成例程模式中才会用到

    //如果返回错误，并且错误的代码不是Pending，说明请求失败
    if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
    {
        QingLog::Write(LL_ERROR, "Post send failed, error = %d.", WSAGetLastError());
        return false;
    }

    return true;
}

bool NetworkServer::ProcessAccept(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext)
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

    LocalComputer MyComputer;
    QingLog::Write(LL_INFO, "(Server %s:%d, Client %s:%d) connected, message = %s.",
        MyComputer.ConvertToIPString(LocalAddr).c_str(),
        ntohs(LocalAddr->sin_port),
        MyComputer.ConvertToIPString(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_WSABuffer.buf);

    //2.拷贝Listen socket上的Context，为新连入的socket创建一个新的SocketContext
    std::shared_ptr<IOCPSocketContext> pNewIOCPSocketContext = std::make_shared<IOCPSocketContext>();
    pNewIOCPSocketContext->m_Socket = pIOCPContext->m_AcceptSocket;
    SetSocketLinger(pNewIOCPSocketContext->m_Socket);
    memcpy(&(pNewIOCPSocketContext->m_ClientAddr), ClientAddr, sizeof(SOCKADDR_IN));

    //将这个新的Socket和完成端口绑定
    if (CreateIoCompletionPort((HANDLE)pNewIOCPSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)(pNewIOCPSocketContext.get()), 0) == NULL)
    {
        QingLog::Write(LL_ERROR, "Associate with IOCP error = %d.", GetLastError());
        return false;
    }

    //3.建立新Socket下的IOContext，用于在这个socket上投递第一个Recv数据请求
    std::shared_ptr<IOCPContext> pNewIOCPContext = pNewIOCPSocketContext->GetNewIOContext();
    pNewIOCPContext->m_ActionType = IOCP_AT_RECV;
    pNewIOCPContext->m_AcceptSocket = pNewIOCPSocketContext->m_Socket;
    memcpy(pNewIOCPContext->m_Buffer, pIOCPContext->m_Buffer, MAX_IO_CONTEXT_BUFFER_LEN);

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

bool NetworkServer::ProcessRecv(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext)
{
    //处理消息
    LocalComputer MyComputer;
    SOCKADDR_IN *ClientAddr = &pSocketContext->m_ClientAddr;
    QingLog::Write(LL_INFO, "Recv %s:%d message = %s",
        MyComputer.ConvertToIPString(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_WSABuffer.buf);

    //开始投递下一个WSARecv请求
    return PostRecv(pIOCPContext);
}

bool NetworkServer::ProcessSend(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext)
{
    return false;
}

DWORD NetworkServer::CallBack_WorkerThread(LPVOID lpParam)
{
    ServerWorkerThreadParam *pParam = (ServerWorkerThreadParam*)lpParam;
    NetworkServer *pQingIOCP = (NetworkServer*)pParam->m_QingServer;
    unsigned long ThreadID = (unsigned long)pParam->m_ThreadID;
    QingLog::Write(LL_INFO, "Worker Thread DEC_ID = %d, HEX_ID = %x start.", ThreadID, ThreadID);

    OVERLAPPED *pOverlapped = NULL;
    DWORD dwBytesTransfered = 0;

    //循环处理，直到收到Shutdown消息为止
    while (WaitForSingleObject(pQingIOCP->m_hWorkerThreadExitEvent, 0) != WAIT_OBJECT_0)
    {
        std::shared_ptr<IOCPSocketContext> pSocketContext;

        BOOL bReturn = GetQueuedCompletionStatus(
            pQingIOCP->m_hIOCompletionPort,     //完成端口
            &dwBytesTransfered,                 //操作完成后返回的字节数
            (PULONG_PTR)&pSocketContext,        //建立完成端口时绑定的自定义结构体参数
            &pOverlapped,                       //连入socket时一起建立的重叠结构
            INFINITE);                          //等待完成端口的超时时间，如果线程不需要做其它事情则设置为INFINITE

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
            //std::shared_ptr<IOCPContext> pIOCPContext(CONTAINING_RECORD(pOverlapped, IOCPContext, m_Overlapped));


            //判断是否有客户端断开了
            if ((0 == dwBytesTransfered) && (IOCP_AT_RECV == pIOCPContext->m_ActionType || IOCP_AT_SEND == pIOCPContext->m_ActionType))
            {
                LocalComputer MyComputer;
                QingLog::Write(LL_ERROR, "Client %s:%d disconnected.",
                    MyComputer.ConvertToIPString(&(pSocketContext->m_ClientAddr)).c_str(),
                    ntohs(pSocketContext->m_ClientAddr.sin_port));

                //释放资源
                pQingIOCP->m_ClientManager.RemoveClient(pSocketContext);
                continue;
            }
            else
            {
                switch (pIOCPContext->m_ActionType)
                {
                case IOCP_AT_ACCEPT:    pQingIOCP->ProcessAccept(pSocketContext, pIOCPContext);        break;
                case IOCP_AT_RECV:      pQingIOCP->ProcessRecv(pSocketContext, pIOCPContext);          break;
                case IOCP_AT_SEND:      pQingIOCP->ProcessSend(pSocketContext, pIOCPContext);          break;
                default:                QingLog::Write("Worker thread action type error");             break;
                }
            }
        }
    }

    QingLog::Write(LL_INFO, "Worker Thread DEC_ID = %d, HEX_ID = %x exit.", ThreadID, ThreadID);
    return 0;
}

QING_NAMESPACE_END
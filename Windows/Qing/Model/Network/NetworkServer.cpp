#include "NetworkServer.h"
#include "..\..\HeaderFiles\QingLog.h"
#include "..\..\HeaderFiles\LocalComputer.h"

QING_NAMESPACE_BEGIN



NetworkServer::NetworkServer() : NetworkBase(),
                                 m_CallBackAcceptEx(NULL),
                                 m_CallBackGetAcceptExSockAddrs(NULL)
{
}

NetworkServer::~NetworkServer()
{
    Stop();
}

bool NetworkServer::Start(const std::string &ServerIP, int Port)
{
    if (NetworkBase::Start(ServerIP, Port))
    {
        if (CreateIOCP() &&
            CreateWorkerThread() &&
            CreateAndStartListen() &&
            InitializeAcceptExCallBack() &&
            StartPostAcceptExIORequest())
        {
            QingLog::Write("Start succeed, NetworkServer ready.", LL_INFO);
            return true;
        }
    }

    return false;
}

void NetworkServer::Stop()
{
    if (m_ListenSocketContext != NULL && m_ListenSocketContext->m_Socket != INVALID_SOCKET)
    {
        NetworkBase::Stop();
        ReleaseSocket(m_ListenSocketContext->m_Socket);

        QingLog::Write("Stop server.", LL_INFO);
    }
}

int NetworkServer::Send(unsigned __int64 ClientID, const void * MessageData, int MessageSize, __int64 Timeout)
{
    std::string ReceMessage((const char*)MessageData);
    std::string ResponseMessage = ReceMessage + "_ACK";

    std::shared_ptr<IOCPSocketContext> ClientContext = m_ClientManager.GetClientContext(ClientID);
    if (ClientContext->m_SendIOCPContext == NULL)
    {
        ClientContext->m_SendIOCPContext = std::make_shared<IOCPContext>(GetNextIOCPContextID());
    }

    std::shared_ptr<IOCPContext> RecvContext = ClientContext->m_SendIOCPContext;
    RecvContext->ResetBuffer();
    RecvContext->m_AcceptSocket = ClientID;
    RecvContext->m_WSABuffer.len = static_cast<ULONG>(ResponseMessage.size());
    memcpy(RecvContext->m_Buffer, ResponseMessage.c_str(), ResponseMessage.size());
    memcpy(RecvContext->m_WSABuffer.buf, ResponseMessage.c_str(), ResponseMessage.size());

    PostSend(*RecvContext);

    return MessageSize;
}

bool NetworkServer::CreateAndStartListen()
{
    m_ListenSocketContext = std::make_shared<IOCPSocketContext>();

    //想要使用重叠IO,初始化Socket时一定要使用WSASocket并带上WSA_FLAG_OVERLAPPED参数
    m_ListenSocketContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_ListenSocketContext->m_Socket == INVALID_SOCKET)
    {
        QingLog::Write(LL_ERROR, "Create listen socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write(LL_INFO, "Create listen socket = %d succeed.", m_ListenSocketContext->m_Socket);

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

    QingLog::Write(LL_INFO, "Listen socket bind succeed, IP = %s, Port = %d.", GetLocalIP().c_str(), m_ServerPort);

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
        std::shared_ptr<IOCPContext> pAcceptIOCPContext = m_ListenSocketContext->GetNewIOContext(GetNextIOCPContextID());
        if (!PostAccept(*pAcceptIOCPContext))
        {
            m_ListenSocketContext->RemoveContext(pAcceptIOCPContext);
            return false;
        }
    }

    QingLog::Write(LL_INFO, "Post %d AcceptEx request.", InitAcceptPostCount);
    return true;
}

void NetworkServer::GetClientAddress(SOCKADDR_IN &ClientAddress, IOCPContext &AcceptIOCPContext)
{
    SOCKADDR_IN *TempClientAddr = NULL;
    SOCKADDR_IN *TempServerAddr = NULL;
    int ClientLen = sizeof(SOCKADDR_IN);
    int ServerLen = sizeof(SOCKADDR_IN);

    m_CallBackGetAcceptExSockAddrs(
        AcceptIOCPContext.m_WSABuffer.buf,
        AcceptIOCPContext.m_WSABuffer.len - (ACCEPTEX_ADDRESS_LENGTH * 2),
        ACCEPTEX_ADDRESS_LENGTH,
        ACCEPTEX_ADDRESS_LENGTH,
        (LPSOCKADDR*)&TempServerAddr,
        &ServerLen,
        (LPSOCKADDR*)&TempClientAddr,
        &ClientLen);

    LocalComputer MyComputer;
    QingLog::Write(LL_INFO, "Connected, socket(%d) = (Server[%s:%d], Client[%s:%d]), message = %s.",
        AcceptIOCPContext.m_AcceptSocket,
        MyComputer.ConvertToIPString(TempServerAddr).c_str(),
        ntohs(TempServerAddr->sin_port),
        MyComputer.ConvertToIPString(TempClientAddr).c_str(),
        ntohs(TempClientAddr->sin_port),
        AcceptIOCPContext.m_WSABuffer.buf);

    memcpy(&ClientAddress, TempClientAddr, sizeof(SOCKADDR_IN));
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

bool NetworkServer::PostAccept(IOCPContext &AcceptIOCPContext)
{
    //为新连入的客户端预先准备好socket(这是与传统accept最大的区别)
    AcceptIOCPContext.m_AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (AcceptIOCPContext.m_AcceptSocket == INVALID_SOCKET)
    {
        QingLog::Write(LL_ERROR, "Create new accept socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    DWORD dwBytes = 0;
    AcceptIOCPContext.m_ActionType = IOCP_AT_ACCEPT;
    SetSocketLinger(AcceptIOCPContext.m_AcceptSocket);

    //投递AcceptEx
    if (!m_CallBackAcceptEx(
        m_ListenSocketContext->m_Socket,    //用来监听的socket
        AcceptIOCPContext.m_AcceptSocket,       //用于接受连接的socket
        AcceptIOCPContext.m_WSABuffer.buf,      //接收缓冲区：客户端发来的第一组数据；server的地址；client的地址
        AcceptIOCPContext.m_WSABuffer.len - (ACCEPTEX_ADDRESS_LENGTH * 2),
        ACCEPTEX_ADDRESS_LENGTH,            //存放本地地址信息的空间大小
        ACCEPTEX_ADDRESS_LENGTH,            //存放远端地址信息的空间大小
        &dwBytes,                           //忽略
        &AcceptIOCPContext.m_Overlapped))       //本次重叠IO所要用到的重叠结构
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            QingLog::Write(LL_ERROR, "Post accept failed, socket = %d, IOCPContextID = %d, error = %d.",
                AcceptIOCPContext.m_AcceptSocket, AcceptIOCPContext.m_ContextID, WSAGetLastError());
            return false;
        }
    }

    QingLog::Write(LL_ERROR, "Post accept succeed, socket = %d, IOCPContextID = %d.",
        AcceptIOCPContext.m_AcceptSocket, AcceptIOCPContext.m_ContextID);
    return true;
}

bool NetworkServer::ProcessAccept(const std::shared_ptr<IOCPSocketContext> &pClientSocketContext, IOCPContext &AcceptIOCPContext)
{
    //1.获取连入客户端的地址信息
    SOCKADDR_IN ClientAddress;
    GetClientAddress(ClientAddress, AcceptIOCPContext);

    //2.拷贝Listen socket上的Context，为新连入的socket创建一个新的SocketContext
    std::shared_ptr<IOCPSocketContext> pNewIOCPSocketContext = std::make_shared<IOCPSocketContext>();
    memcpy(&(pNewIOCPSocketContext->m_ClientAddr), &ClientAddress, sizeof(SOCKADDR_IN));
    pNewIOCPSocketContext->m_Socket = AcceptIOCPContext.m_AcceptSocket;

    //将这个新的Socket和完成端口绑定
    if (CreateIoCompletionPort((HANDLE)pNewIOCPSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)(pNewIOCPSocketContext.get()), 0) == NULL)
    {
        QingLog::Write(LL_ERROR, "Process accept, associate with IOCP error = %d.", GetLastError());
        return false;
    }

    //3.建立新Socket下的IOContext，用于在这个socket上投递第一个Recv数据请求
    std::shared_ptr<IOCPContext> pNewIOCPContext = pNewIOCPSocketContext->GetNewIOContext(GetNextIOCPContextID());
    memcpy(pNewIOCPContext->m_Buffer, AcceptIOCPContext.m_Buffer, MAX_IO_CONTEXT_BUFFER_LEN);
    pNewIOCPContext->m_AcceptSocket = pNewIOCPSocketContext->m_Socket;
    AcceptIOCPContext.m_AcceptSocket = INVALID_SOCKET;

    QingLog::Write(LL_INFO, "Create new client, socket = %d, IOCPContextID = %I64d",
        pNewIOCPContext->m_AcceptSocket, pNewIOCPContext->m_ContextID);

    //绑定完成之后，就可以开始在这个socket上投递完成请求了
    if (!PostRecv(*pNewIOCPContext))
    {
        pNewIOCPSocketContext->RemoveContext(pNewIOCPContext);
        return false;
    }

    //4.如果投递成功，则记录这个有效的客户端信息
    m_ClientManager.AddClient(pNewIOCPSocketContext);

    //5.重置listen socket，准备投递新的AcceptEx
    AcceptIOCPContext.ResetBuffer();
    return PostAccept(AcceptIOCPContext);
}

bool NetworkServer::ProcessRecv(const std::shared_ptr<IOCPSocketContext> &pClientSocketContext, IOCPContext &RecvIOCPContext)
{
    //处理消息
    LocalComputer MyComputer;
    SOCKADDR_IN *ClientAddr = &pClientSocketContext->m_ClientAddr;
    QingLog::Write(LL_INFO, "Recv, socket = %d, IOCPContextID =%I64d, Client = %s:%d, message = %s",
        RecvIOCPContext.m_AcceptSocket,
        RecvIOCPContext.m_ContextID,
        MyComputer.ConvertToIPString(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        RecvIOCPContext.m_WSABuffer.buf);

    Send(RecvIOCPContext.m_AcceptSocket, RecvIOCPContext.m_WSABuffer.buf, RecvIOCPContext.m_WSABuffer.len);

    //开始投递下一个WSARecv请求
    return PostRecv(RecvIOCPContext);
}

bool NetworkServer::ProcessSend(const std::shared_ptr<IOCPSocketContext> &pClientSocketContext, IOCPContext &SendIOCPContext)
{
    QingLog::Write(LL_INFO, "Send message = %s", SendIOCPContext.m_WSABuffer.buf);
    return false;
}

void NetworkServer::WorkerThread()
{
    DWORD dwBytesTransfered = 0;
    OVERLAPPED *pOverlapped = NULL;

    //循环处理，直到收到Shutdown消息为止
    while (WaitForSingleObject(m_hWorkerThreadExitEvent, 0) != WAIT_OBJECT_0)
    {
        std::shared_ptr<IOCPSocketContext> pClientSocketContext;
        BOOL bReturn = GetQueuedCompletionStatus(
            m_hIOCompletionPort,                    //完成端口
            &dwBytesTransfered,                     //操作完成后返回的字节数
            (PULONG_PTR)&pClientSocketContext,      //建立完成端口时绑定的自定义结构体参数
            &pOverlapped,                           //连入socket时一起建立的重叠结构
            INFINITE);                              //等待完成端口的超时时间，如果线程不需要做其它事情则设置为INFINITE(永远不会超时)

        if (pClientSocketContext == NULL)           //如果收到的是退出消息则直接退出
        {
            break;
        }

        if (!bReturn || pOverlapped == NULL)
        {
            if (!HandleError(pClientSocketContext, GetLastError()))
            {
                break;
            }

            continue;
        }

        //读取传入的参数
        IOCPContext *pIOCPContext(CONTAINING_RECORD(pOverlapped, IOCPContext, m_Overlapped));

        //判断是否有客户端断开了
        if ((0 == dwBytesTransfered) && (IOCP_AT_RECV == pIOCPContext->m_ActionType || IOCP_AT_SEND == pIOCPContext->m_ActionType))
        {
            LocalComputer MyComputer;
            QingLog::Write(LL_ERROR, "Client %s:%d disconnected.",
                MyComputer.ConvertToIPString(&(pClientSocketContext->m_ClientAddr)).c_str(),
                ntohs(pClientSocketContext->m_ClientAddr.sin_port));

            //释放资源
            m_ClientManager.RemoveClient(pClientSocketContext);
            continue;
        }

        //处理具体请求
        switch (pIOCPContext->m_ActionType)
        {
        case IOCP_AT_ACCEPT:    ProcessAccept(pClientSocketContext, *pIOCPContext);        break;
        case IOCP_AT_RECV:      ProcessRecv(pClientSocketContext, *pIOCPContext);          break;
        case IOCP_AT_SEND:      ProcessSend(pClientSocketContext, *pIOCPContext);          break;
        default:                QingLog::Write("Worker thread action type error.");        break;
        }
    }
}

QING_NAMESPACE_END
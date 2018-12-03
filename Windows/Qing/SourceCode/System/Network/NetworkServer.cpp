#include "NetworkServer.h"
#include "NetworkEnvironment.h"
#include "..\..\Boost\BoostLog.h"
#include "..\..\Boost\BoostFormat.h"
#include "..\LocalComputer.h"

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
        if (CreateWorkerThread() &&
            CreateAndStartListen() &&
            InitializeAcceptExCallBack() &&
            StartPostAcceptExIORequest())
        {
            BoostLog::WriteInfo(L"Start succeed, NetworkServer ready.");
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

        BoostLog::WriteInfo(L"Stop server.");
    }
}

int NetworkServer::Send(SOCKET ClientID, const void * MessageData, int MessageSize, __int64 Timeout)
{
    std::vector<SOCKET> ClientVector;
    ClientID <= 0 ? m_ClientManager.GetAllClientID(ClientVector): ClientVector.push_back(ClientID);

    for (std::vector<SOCKET>::size_type Index = 0; Index < ClientVector.size(); Index++)
    {
        std::shared_ptr<IOCPSocketContext> ClientContext = m_ClientManager.GetClientContext(ClientVector[Index]);
        std::shared_ptr<IOCPContext> &SendIOCPContext = ClientContext->GetNewIOContext(GlobalNetwork.GetNextTrackID());

        SendIOCPContext->m_AcceptSocket = ClientVector[Index];
        SendIOCPContext->m_WSABuffer.len = MessageSize;
        memcpy(SendIOCPContext->m_Buffer, MessageData, MessageSize);
        memcpy(SendIOCPContext->m_WSABuffer.buf, MessageData, MessageSize);
        PostSend(*SendIOCPContext);
    }

    return ClientVector.empty() ? 0 : MessageSize;
}

bool NetworkServer::CreateAndStartListen()
{
    m_ListenSocketContext = std::make_shared<IOCPSocketContext>();

    //��Ҫʹ���ص�IO,��ʼ��Socketʱһ��Ҫʹ��WSASocket������WSA_FLAG_OVERLAPPED����
    m_ListenSocketContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_ListenSocketContext->m_Socket == INVALID_SOCKET)
    {
        BoostLog::WriteError(BoostFormat(L"Create listen socket failed, error = %d.", WSAGetLastError()));
        return false;
    }

    BoostLog::WriteInfo(BoostFormat(L"Create listen Socket = %I64d succeed.", m_ListenSocketContext->m_Socket));

    //��listen socket�󶨵���ɶ˿�
    //����������CompletionKey�������̲߳�������worker�߳��оͿ���ʹ�����������
    if (CreateIoCompletionPort((HANDLE)m_ListenSocketContext->m_Socket, GlobalNetwork.GetIOCP(), (ULONG_PTR)(m_ListenSocketContext.get()), 0) == NULL)
    {
        BoostLog::WriteError(BoostFormat(L"Bind listen socket to IOCP failed, error = %d.", WSAGetLastError()));
        return false;
    }

    BoostLog::WriteInfo(L"Bind listen socket to IOCP succeed.");

    //�󶨵�ַ�Ͷ˿�
    struct sockaddr_in ServerAddress;
    FillAddress(ServerAddress);
    if (bind(m_ListenSocketContext->m_Socket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
    {
        BoostLog::WriteError(BoostFormat(L"Listen socket bind failed, error = %d.", WSAGetLastError()));
        return false;
    }

    BoostLog::WriteInfo(BoostFormat(L"Listen socket bind succeed, IP = %s, Port = %d.", GetLocalIP().c_str(), m_ServerPort));

    //��ʼ���м���
    //SOMAXCONN:Maximum queue length specifiable by listen
    if (listen(m_ListenSocketContext->m_Socket, SOMAXCONN) == SOCKET_ERROR)
    {
        BoostLog::WriteError(BoostFormat(L"Listen error = %d.", WSAGetLastError()));
        return false;
    }

    BoostLog::WriteInfo(L"Listening...");
    return true;
}

bool NetworkServer::InitializeAcceptExCallBack()
{
    //AcceptEx������Winsock2��ϵ�����ÿ�ζ�ֱ�ӵ�������Service Providerÿ��
    //��Ҫͨ��WSAIoctl()��ȡ�ú�����ָ�룬������ֱ���ڴ������Ȼ�ȡһ������ָ�롣
    //GetAcceptExSockAddrs����ͬ��

    //��ȡAcceptEx����ָ��
    DWORD dwBytes = 0;
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    if (SOCKET_ERROR == WSAIoctl(
        m_ListenSocketContext->m_Socket,        //��socket�޹أ���Ϊֻ�ǻ�ȡ����ָ��
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidAcceptEx,                          //�Ѿ�����õ�GUID
        sizeof(GuidAcceptEx),
        &m_CallBackAcceptEx,
        sizeof(m_CallBackAcceptEx),
        &dwBytes,
        NULL,
        NULL))
    {
        BoostLog::WriteError(BoostFormat(L"WSAIoctl can not get AcceptEx pointer, error = %d.", WSAGetLastError()));
        return false;
    }

    //��ȡGetAcceptExSockAddrs����ָ��
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    if (SOCKET_ERROR == WSAIoctl(
        m_ListenSocketContext->m_Socket,        //��socket�޹أ���Ϊֻ�ǻ�ȡ����ָ��
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidGetAcceptExSockAddrs,              //�Ѿ�����õ�GUID
        sizeof(GuidGetAcceptExSockAddrs),
        &m_CallBackGetAcceptExSockAddrs,
        sizeof(m_CallBackGetAcceptExSockAddrs),
        &dwBytes,
        NULL,
        NULL))
    {
        BoostLog::WriteError(BoostFormat(L"WSAIoctl can not get GetAcceptExSockAddrs pointer, error = %d.", WSAGetLastError()));
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
        std::shared_ptr<IOCPContext> pAcceptIOCPContext = m_ListenSocketContext->GetNewIOContext(GlobalNetwork.GetNextTrackID());
        if (!PostAccept(*pAcceptIOCPContext))
        {
            m_ListenSocketContext->DeleteContext(*pAcceptIOCPContext);
            return false;
        }
    }

    BoostLog::WriteInfo(BoostFormat(L"Post %d AcceptEx request.", InitAcceptPostCount));
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
    BoostLog::WriteInfo(BoostFormat(L"Connected, Socket(%I64d) = (Server[%s:%d], Client[%s:%d]), message = %s.",
        AcceptIOCPContext.m_AcceptSocket,
        MyComputer.ConvertToIPString(TempServerAddr).c_str(),
        ntohs(TempServerAddr->sin_port),
        MyComputer.ConvertToIPString(TempClientAddr).c_str(),
        ntohs(TempClientAddr->sin_port),
        AcceptIOCPContext.m_WSABuffer.buf));

    memcpy(&ClientAddress, TempClientAddr, sizeof(SOCKADDR_IN));
}

bool NetworkServer::IsSocketAlive(SOCKET socket)
{
    //�жϿͻ���socket�Ƿ��Ѿ��Ͽ���������һ����Чsocket��Ͷ��WSARecv�������׳��쳣
    //�жϷ����ǳ��������socket�������ݣ��ж����socket���õķ���ֵ
    //��Ϊ����ͻ��������쳣�Ͽ�(������ε�����)ʱ������޷��յ��ͻ��˶Ͽ���֪ͨ

    int nByteSend = send(socket, "", 0, 0);
    return (nByteSend != -1);
}

bool NetworkServer::HandleError(const std::shared_ptr<IOCPSocketContext> &pSocketContext, DWORD ErrorCode)
{
    if (WAIT_TIMEOUT == ErrorCode)                          //��ʱ
    {
        if (IsSocketAlive(pSocketContext->m_Socket))
        {
            BoostLog::WriteInfo(L"QingNetwork time out, reconnecting....");
            return true;
        }
        else
        {
            BoostLog::WriteInfo(L"Client disconnected.");
            m_ClientManager.RemoveClient(pSocketContext);
            return true;
        }
    }
    else if (ERROR_NETNAME_DELETED == ErrorCode)            //�쳣
    {
        BoostLog::WriteInfo(L"Client disconnected.");
        m_ClientManager.RemoveClient(pSocketContext);
        return true;
    }
    else
    {
        BoostLog::WriteError(BoostFormat(L"IOCP error = %d, exit thread.", ErrorCode));
        return false;
    }
}

bool NetworkServer::PostAccept(IOCPContext &AcceptIOCPContext)
{
    //Ϊ������Ŀͻ���Ԥ��׼����socket(�����봫ͳaccept��������)
    AcceptIOCPContext.m_AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (AcceptIOCPContext.m_AcceptSocket == INVALID_SOCKET)
    {
        BoostLog::WriteError(BoostFormat(L"Create new accept socket failed, error = %d.", WSAGetLastError()));
        return false;
    }

    DWORD dwBytes = 0;
    AcceptIOCPContext.m_ActionType = IOCP_AT_ACCEPT;
    SetSocketLinger(AcceptIOCPContext.m_AcceptSocket);

    //Ͷ��AcceptEx
    if (!m_CallBackAcceptEx(
        m_ListenSocketContext->m_Socket,        //����������socket
        AcceptIOCPContext.m_AcceptSocket,       //���ڽ������ӵ�socket
        AcceptIOCPContext.m_WSABuffer.buf,      //���ջ��������ͻ��˷����ĵ�һ�����ݣ�server�ĵ�ַ��client�ĵ�ַ
        AcceptIOCPContext.m_WSABuffer.len - (ACCEPTEX_ADDRESS_LENGTH * 2),
        ACCEPTEX_ADDRESS_LENGTH,                //��ű��ص�ַ��Ϣ�Ŀռ��С
        ACCEPTEX_ADDRESS_LENGTH,                //���Զ�˵�ַ��Ϣ�Ŀռ��С
        &dwBytes,                               //����
        &AcceptIOCPContext.m_Overlapped))       //�����ص�IO��Ҫ�õ����ص��ṹ
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            BoostLog::WriteError(BoostFormat(L"Post accept failed, Socket = %I64d, IOCPContextID = %I64d, error = %d.",
                AcceptIOCPContext.m_AcceptSocket, AcceptIOCPContext.m_ContextID, WSAGetLastError()));
            return false;
        }
    }

    BoostLog::WriteInfo(BoostFormat(L"Post accept succeed, Socket = %I64d, IOCPContextID = %I64d.",
        AcceptIOCPContext.m_AcceptSocket, AcceptIOCPContext.m_ContextID));
    return true;
}

bool NetworkServer::ProcessAccept(const std::shared_ptr<IOCPSocketContext> &pClientSocketContext, IOCPContext &AcceptIOCPContext)
{
    //1.��ȡ����ͻ��˵ĵ�ַ��Ϣ
    SOCKADDR_IN ClientAddress;
    GetClientAddress(ClientAddress, AcceptIOCPContext);

    //2.����Listen socket�ϵ�Context��Ϊ�������socket����һ���µ�SocketContext
    std::shared_ptr<IOCPSocketContext> pNewIOCPSocketContext = std::make_shared<IOCPSocketContext>();
    memcpy(&(pNewIOCPSocketContext->m_RemoteAddr), &ClientAddress, sizeof(SOCKADDR_IN));
    pNewIOCPSocketContext->m_Socket = AcceptIOCPContext.m_AcceptSocket;

    //������µ�Socket����ɶ˿ڰ�
    if (CreateIoCompletionPort((HANDLE)pNewIOCPSocketContext->m_Socket, GlobalNetwork.GetIOCP(), (ULONG_PTR)(pNewIOCPSocketContext.get()), 0) == NULL)
    {
        BoostLog::WriteError(BoostFormat(L"Process accept, associate with IOCP error = %d.", GetLastError()));
        return false;
    }

    //3.������Socket�µ�IOContext�����������socket��Ͷ�ݵ�һ��Recv��������
    pNewIOCPSocketContext->m_RecvIOCPContext = std::make_shared<IOCPContext>(GlobalNetwork.GetNextTrackID());
    memcpy(pNewIOCPSocketContext->m_RecvIOCPContext->m_Buffer, AcceptIOCPContext.m_Buffer, MAX_IO_CONTEXT_BUFFER_LEN);
    pNewIOCPSocketContext->m_RecvIOCPContext->m_AcceptSocket = pNewIOCPSocketContext->m_Socket;

    //�����֮�󣬾Ϳ��Կ�ʼ�����socket��Ͷ�����������
    if (!PostRecv(*(pNewIOCPSocketContext->m_RecvIOCPContext)))
    {
        return false;
    }

    //4.���Ͷ�ݳɹ������¼�����Ч�Ŀͻ�����Ϣ
    m_ClientManager.AddClient(pNewIOCPSocketContext);

    //5.����listen socket��׼��Ͷ���µ�AcceptEx
    AcceptIOCPContext.ResetBuffer();
    return PostAccept(AcceptIOCPContext);
}

bool NetworkServer::ProcessRecv(const std::shared_ptr<IOCPSocketContext> &pClientSocketContext, IOCPContext &RecvIOCPContext)
{
    //������Ϣ
    LocalComputer MyComputer;
    SOCKADDR_IN *ClientAddr = &pClientSocketContext->m_RemoteAddr;
    BoostLog::WriteInfo(BoostFormat(L"Recv, Socket = %I64d, IOCPContextID =%I64d, Client = %s:%d, message = %s",
        RecvIOCPContext.m_AcceptSocket,
        RecvIOCPContext.m_ContextID,
        MyComputer.ConvertToIPString(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        RecvIOCPContext.m_WSABuffer.buf));

    //ACK
    std::string ResponseMessage("ACK_");
    ResponseMessage += RecvIOCPContext.m_WSABuffer.buf;
    Send(RecvIOCPContext.m_AcceptSocket, ResponseMessage.c_str(), static_cast<int>(ResponseMessage.size()));

    //��ʼͶ����һ��WSARecv����
    return PostRecv(RecvIOCPContext);
}

bool NetworkServer::ProcessSend(const std::shared_ptr<IOCPSocketContext> &pClientSocketContext, IOCPContext &SendIOCPContext)
{
    BoostLog::WriteInfo(BoostFormat(L"Send message = %s", SendIOCPContext.m_WSABuffer.buf));
    pClientSocketContext->DeleteContext(SendIOCPContext);
    return true;
}

void NetworkServer::WorkerThread()
{
    DWORD dwBytesTransfered = 0;
    OVERLAPPED *pOverlapped = NULL;

    //ѭ������ֱ���յ�Shutdown��ϢΪֹ
    while (WaitForSingleObject(m_hWorkerThreadExitEvent, 0) != WAIT_OBJECT_0)
    {
        std::shared_ptr<IOCPSocketContext> pClientSocketContext;
        BOOL bReturn = GetQueuedCompletionStatus(
            GlobalNetwork.GetIOCP(),                //��ɶ˿�
            &dwBytesTransfered,                     //������ɺ󷵻ص��ֽ���
            (PULONG_PTR)&pClientSocketContext,      //������ɶ˿�ʱ�󶨵��Զ���ṹ�����
            &pOverlapped,                           //����socketʱһ�������ص��ṹ
            INFINITE);                              //�ȴ���ɶ˿ڵĳ�ʱʱ�䣬����̲߳���Ҫ����������������ΪINFINITE(��Զ���ᳬʱ)

        if (pClientSocketContext == NULL)           //����յ������˳���Ϣ��ֱ���˳�
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

        //��ȡ����Ĳ���
        IOCPContext *pIOCPContext(CONTAINING_RECORD(pOverlapped, IOCPContext, m_Overlapped));

        //�ж��Ƿ��пͻ��˶Ͽ���
        if ((0 == dwBytesTransfered) && (IOCP_AT_RECV == pIOCPContext->m_ActionType || IOCP_AT_SEND == pIOCPContext->m_ActionType))
        {
            LocalComputer MyComputer;
            BoostLog::WriteError(BoostFormat(L"Client %s:%d disconnected.",
                MyComputer.ConvertToIPString(&(pClientSocketContext->m_RemoteAddr)).c_str(),
                ntohs(pClientSocketContext->m_RemoteAddr.sin_port)));

            //�ͷ���Դ
            m_ClientManager.RemoveClient(pClientSocketContext);
            continue;
        }

        //�����������
        switch (pIOCPContext->m_ActionType)
        {
        case IOCP_AT_ACCEPT:    ProcessAccept(pClientSocketContext, *pIOCPContext);        break;
        case IOCP_AT_RECV:      ProcessRecv(pClientSocketContext, *pIOCPContext);          break;
        case IOCP_AT_SEND:      ProcessSend(pClientSocketContext, *pIOCPContext);          break;
        default:                BoostLog::WriteError(L"Worker thread action type error."); break;
        }
    }
}

QING_NAMESPACE_END
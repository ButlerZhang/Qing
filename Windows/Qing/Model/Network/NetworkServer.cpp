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
        QingLog::Write("Stop server.", LL_INFO);
    }
}

int NetworkServer::Send(int ClientID, const void * MessageData, int MessageSize, __int64 Timeout)
{
    return 0;
}

void NetworkServer::WorkerThread()
{
    OVERLAPPED *pOverlapped = NULL;
    DWORD dwBytesTransfered = 0;

    //ѭ������ֱ���յ�Shutdown��ϢΪֹ
    while (WaitForSingleObject(m_hWorkerThreadExitEvent, 0) != WAIT_OBJECT_0)
    {
        std::shared_ptr<IOCPSocketContext> pSocketContext;

        BOOL bReturn = GetQueuedCompletionStatus(
            m_hIOCompletionPort,                //��ɶ˿�
            &dwBytesTransfered,                 //������ɺ󷵻ص��ֽ���
            (PULONG_PTR)&pSocketContext,        //������ɶ˿�ʱ�󶨵��Զ���ṹ�����
            &pOverlapped,                       //����socketʱһ�������ص��ṹ
            INFINITE);                          //�ȴ���ɶ˿ڵĳ�ʱʱ�䣬����̲߳���Ҫ����������������ΪINFINITE(��Զ���ᳬʱ)

        if (pSocketContext == NULL)             //����յ������˳���Ϣ��ֱ���˳�
        {
            break;
        }

        if (!bReturn)
        {
            if (!HandleError(pSocketContext, GetLastError()))
            {
                break;
            }

            continue;
        }
        else
        {
            //��ȡ����Ĳ���
            IOCPContext *pIOCPContext(CONTAINING_RECORD(pOverlapped, IOCPContext, m_Overlapped));

            //�ж��Ƿ��пͻ��˶Ͽ���
            if ((0 == dwBytesTransfered) && (IOCP_AT_RECV == pIOCPContext->m_ActionType || IOCP_AT_SEND == pIOCPContext->m_ActionType))
            {
                LocalComputer MyComputer;
                QingLog::Write(LL_ERROR, "Client %s:%d disconnected.",
                    MyComputer.ConvertToIPString(&(pSocketContext->m_ClientAddr)).c_str(),
                    ntohs(pSocketContext->m_ClientAddr.sin_port));

                //�ͷ���Դ
                m_ClientManager.RemoveClient(pSocketContext);
                continue;
            }
            else
            {
                switch (pIOCPContext->m_ActionType)
                {
                case IOCP_AT_ACCEPT:    ProcessAccept(pSocketContext, pIOCPContext);        break;
                case IOCP_AT_RECV:      ProcessRecv(pSocketContext, pIOCPContext);          break;
                case IOCP_AT_SEND:      ProcessSend(pSocketContext, pIOCPContext);          break;
                default:                QingLog::Write("Worker thread action type error");  break;
                }
            }
        }
    }
}

bool NetworkServer::CreateAndStartListen()
{
    m_ListenSocketContext = std::make_shared<IOCPSocketContext>();

    //��Ҫʹ���ص�IO,��ʼ��Socketʱһ��Ҫʹ��WSASocket������WSA_FLAG_OVERLAPPED����
    m_ListenSocketContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_ListenSocketContext->m_Socket == INVALID_SOCKET)
    {
        QingLog::Write(LL_ERROR, "Initialize listen socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write(LL_INFO, "Initialize listen socket = %d succeed.", m_ListenSocketContext->m_Socket);

    //��listen socket�󶨵���ɶ˿�
    //����������CompletionKey�������̲߳�������worker�߳��оͿ���ʹ�����������
    if (CreateIoCompletionPort((HANDLE)m_ListenSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)(m_ListenSocketContext.get()), 0) == NULL)
    {
        QingLog::Write(LL_ERROR, "Bind listen socket to IOCP failed, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write("Bind listen socket to IOCP succeed.", LL_INFO);

    //�󶨵�ַ�Ͷ˿�
    struct sockaddr_in ServerAddress;
    FillAddress(ServerAddress);
    if (bind(m_ListenSocketContext->m_Socket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
    {
        QingLog::Write(LL_ERROR, "Listen socket bind failed, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write(LL_INFO, "Listen socket bind succeed, IP = %s, Port = %d.", GetLocalIP().c_str(), m_ServerPort);

    //��ʼ���м���
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
        QingLog::Write(LL_ERROR, "WSAIoctl can not get AcceptEx pointer, error = %d.", WSAGetLastError());
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
    else if (ERROR_NETNAME_DELETED == ErrorCode)            //�쳣
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
    //Ϊ������Ŀͻ�����׼����socket(�����봫ͳaccept��������)
    pIOCPContext->m_AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (pIOCPContext->m_AcceptSocket == INVALID_SOCKET)
    {
        QingLog::Write(LL_ERROR, "Create new accept socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    DWORD dwBytes = 0;
    pIOCPContext->m_ActionType = IOCP_AT_ACCEPT;

    //Ͷ��AcceptEx
    if (!m_CallBackAcceptEx(
        m_ListenSocketContext->m_Socket,    //����������socket
        pIOCPContext->m_AcceptSocket,       //���ڽ������ӵ�socket
        pIOCPContext->m_WSABuffer.buf,      //���ջ��������ͻ��˷����ĵ�һ�����ݣ�server�ĵ�ַ��client�ĵ�ַ
        pIOCPContext->m_WSABuffer.len - (ACCEPTEX_ADDRESS_LENGTH * 2),
        ACCEPTEX_ADDRESS_LENGTH,            //��ű��ص�ַ��Ϣ�Ŀռ��С
        ACCEPTEX_ADDRESS_LENGTH,            //���Զ�˵�ַ��Ϣ�Ŀռ��С
        &dwBytes,                           //����
        &pIOCPContext->m_Overlapped))       //�����ص�IO��Ҫ�õ����ص��ṹ
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            QingLog::Write(LL_ERROR, "Post accept failed, error = %d.", WSAGetLastError());
            return false;
        }
    }

    QingLog::Write(LL_INFO, "Post accept socket = %d succeed.", pIOCPContext->m_AcceptSocket);
    return true;
}

bool NetworkServer::PostRecv(IOCPContext *pIOCPContext)
{
    pIOCPContext->ResetBuffer();
    pIOCPContext->m_ActionType = IOCP_AT_RECV;

    //Ͷ��WSARecv����
    DWORD dwFlags = 0, dwBytes = 0;
    int nBytesRecv = WSARecv(
        pIOCPContext->m_AcceptSocket,   //Ͷ�����������socket
        &pIOCPContext->m_WSABuffer,     //���ջ�������WSABUF�ṹ���ɵ�����
        1,                              //������WSABUF�ṹ����������Ϊ1
        &dwBytes,                       //���յ����ֽ���
        &dwFlags,                       //����Ϊ0
        &pIOCPContext->m_Overlapped,    //���socket��Ӧ���ص��ṹ
        NULL);                          //�������ֻ�����������ģʽ�вŻ��õ�

    //������ش��󣬲��Ҵ���Ĵ��벻��Pending��˵������ʧ��
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

    //Ͷ��WSASend����
    DWORD dwFlags = 0, dwBytes = 0;
    int nBytesRecv = WSASend(
        pIOCPContext->m_AcceptSocket,   //Ͷ�����������socket
        &pIOCPContext->m_WSABuffer,     //���ջ�������WSABUF�ṹ���ɵ�����
        1,                              //������WSABUF�ṹ����������Ϊ1
        &dwBytes,                       //���յ����ֽ���
        dwFlags,                        //����Ϊ0
        &pIOCPContext->m_Overlapped,    //���socket��Ӧ���ص��ṹ
        NULL);                          //�������ֻ�����������ģʽ�вŻ��õ�

    //������ش��󣬲��Ҵ���Ĵ��벻��Pending��˵������ʧ��
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

    //1.��ȡ����ͻ��˵ĵ�ַ��Ϣ
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
    QingLog::Write(LL_INFO, "socket(%d) = (Server %s:%d, Client %s:%d) connected, message = %s.",
        pIOCPContext->m_AcceptSocket,
        MyComputer.ConvertToIPString(LocalAddr).c_str(),
        ntohs(LocalAddr->sin_port),
        MyComputer.ConvertToIPString(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_WSABuffer.buf);

    //2.����Listen socket�ϵ�Context��Ϊ�������socket����һ���µ�SocketContext
    std::shared_ptr<IOCPSocketContext> pNewIOCPSocketContext = std::make_shared<IOCPSocketContext>();
    pNewIOCPSocketContext->m_Socket = pIOCPContext->m_AcceptSocket;
    SetSocketLinger(pNewIOCPSocketContext->m_Socket);
    memcpy(&(pNewIOCPSocketContext->m_ClientAddr), ClientAddr, sizeof(SOCKADDR_IN));

    //������µ�Socket����ɶ˿ڰ�
    if (CreateIoCompletionPort((HANDLE)pNewIOCPSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)(pNewIOCPSocketContext.get()), 0) == NULL)
    {
        QingLog::Write(LL_ERROR, "Associate with IOCP error = %d.", GetLastError());
        return false;
    }

    //3.������Socket�µ�IOContext�����������socket��Ͷ�ݵ�һ��Recv��������
    std::shared_ptr<IOCPContext> pNewIOCPContext = pNewIOCPSocketContext->GetNewIOContext();
    pNewIOCPContext->m_ActionType = IOCP_AT_RECV;
    pNewIOCPContext->m_AcceptSocket = pNewIOCPSocketContext->m_Socket;
    memcpy(pNewIOCPContext->m_Buffer, pIOCPContext->m_Buffer, MAX_IO_CONTEXT_BUFFER_LEN);

    //�����֮�󣬾Ϳ��Կ�ʼ�����socket��Ͷ�����������
    if (!PostRecv(pNewIOCPContext.get()))
    {
        pNewIOCPSocketContext->RemoveContext(pNewIOCPContext);
        return false;
    }

    //4.���Ͷ�ݳɹ������¼�����Ч�Ŀͻ�����Ϣ
    m_ClientManager.AddClient(pNewIOCPSocketContext);

    //5.����listen socket��׼��Ͷ���µ�AcceptEx
    pIOCPContext->ResetBuffer();
    return PostAccept(pIOCPContext);
}

bool NetworkServer::ProcessRecv(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext)
{
    //������Ϣ
    LocalComputer MyComputer;
    SOCKADDR_IN *ClientAddr = &pSocketContext->m_ClientAddr;
    QingLog::Write(LL_INFO, "socket(%d) Recv %s:%d message = %s",
        pIOCPContext->m_AcceptSocket,
        MyComputer.ConvertToIPString(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_WSABuffer.buf);

    //��ʼͶ����һ��WSARecv����
    return PostRecv(pIOCPContext);
}

bool NetworkServer::ProcessSend(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext)
{
    return false;
}

QING_NAMESPACE_END
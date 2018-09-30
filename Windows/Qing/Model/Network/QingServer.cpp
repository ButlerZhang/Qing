#include "QingServer.h"
#include "..\..\HeaderFiles\QingLog.h"
#include "..\..\HeaderFiles\LocalComputer.h"
#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN


struct WorkerThreadParam
{
    QingServer    *m_pQingIOCP;       //���ڵ������еĺ���
    int          m_nThreadNo;       //�̱߳��
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

    //�����߳��˳����¼�֪ͨ
    m_hShutdownEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

    //��ʼ��IOCP
    if (!InitializeIOCP())
    {
        Qing::QingLog::Write("Initialize IOCP failed.", Qing::LL_ERROR);
        return false;
    }

    Qing::QingLog::Write("Initialize IOCP succeed.", Qing::LL_INFO);

    //��ʼ��Socket
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
        //����ر���Ϣ֪ͨ
        SetEvent(m_hShutdownEvent);

        //֪ͨ���е���ɶ˿ڲ����˳�
        for (int i = 0; i < m_WorkerThreadCount; i++)
        {
            PostQueuedCompletionStatus(m_hIOCompletionPort, 0, NULL, NULL);
        }

        //�ȴ����еĿͻ�����Դ�˳�
        WaitForMultipleObjects(m_WorkerThreadCount, m_phWorkerThreads, TRUE, INFINITE);

        //�ͷ�������Դ
        DeleteInitializeResources();

        Qing::QingLog::Write("Stop listen.", Qing::LL_INFO);
    }
}

const std::string& QingServer::GetLocalIP()
{
    //char HostName[MAX_PATH];
    //memset(HostName, 0, MAX_PATH);

    ////��ȡ������
    //gethostname(HostName, MAX_PATH);
    //hostent FAR* lpHostEnt = gethostbyname(HostName);
    //if (lpHostEnt == NULL)
    //{
    //    return m_ServerIP;
    //}

    ////��ȡIP��ַ�б��еĵ�һ��IP
    //LPSTR lpAddr = lpHostEnt->h_addr_list[0];

    ////��IP��ַת�����ַ�����ʽ
    //struct in_addr inAddr;
    //memmove(&inAddr, lpAddr, 4);
    //m_ServerIP = inet_ntoa(inAddr);

    return m_ServerBindIP;
}

bool QingServer::InitializeIOCP()
{
    //������ɶ˿�
    m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == m_hIOCompletionPort)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Create IO completion port error = %d.", WSAGetLastError());
        return false;
    }

    //���ݴ���������������Ӧ���߳���
    Qing::LocalComputer MyComputer;
    m_WorkerThreadCount = WORKER_THREADS_PER_PROCESSOR * MyComputer.GetProcessorsCount();
    m_phWorkerThreads = new HANDLE[m_WorkerThreadCount];

    //����worker�߳�
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
    //�������ڼ�����socket��Ϣ
    m_ListenSocketContext = std::make_shared<IOCPSocketContext>();

    //ʹ��WSASocket����socket���ſ���֧���ص�IO����
    m_ListenSocketContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_ListenSocketContext->m_Socket == INVALID_SOCKET)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Initialize listen socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Initialize listen socket succeed.", Qing::LL_INFO);

    //��listen socket�󶨵���ɶ˿�
    if (CreateIoCompletionPort((HANDLE)m_ListenSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)(m_ListenSocketContext.get()), 0) == NULL)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Bind listen socket to IOCP failed, error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Bind listen socket to IOCP succeed.", Qing::LL_INFO);

    //����ַ��Ϣ
    struct sockaddr_in ServerAddress;
    ZeroMemory((char*)&ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(m_ListenPort);

    if (m_ServerBindIP.empty())
    {
        //���κ�һ����ַ
        ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        //��ĳ��IP��ַ
        inet_pton(AF_INET, m_ServerBindIP.c_str(), &(ServerAddress.sin_addr.s_addr));
    }

    //�󶨵�ַ�Ͷ˿�
    if (bind(m_ListenSocketContext->m_Socket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Listen socket bind failed, error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Listen socket bind succeed.", Qing::LL_INFO);

    //��ʼ���м���
    if (listen(m_ListenSocketContext->m_Socket, SOMAXCONN) == SOCKET_ERROR)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Listen error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Listening...", Qing::LL_INFO);

    //���ڵ�������ָ��
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;

    //ʹ��AcceptEx��������Ϊ�����WinSock2�淶֮�����չ������
    //������Ҫ�����ȡһ�º�����ָ��
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

    //��ȡGetAcceptExSockAddrs����ָ��
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

    //ΪAcceptEx׼��������Ȼ��Ͷ��AcceptEx IO����
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

    //Ϊ������Ŀͻ�����׼����socket(�����봫ͳaccept��������)
    pIOCPContext->m_AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (pIOCPContext->m_AcceptSocket == INVALID_SOCKET)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Create accept socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    //Ͷ��AcceptEx
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

    //Ͷ��WSARecv����
    int nBytesRecv = WSARecv(pIOCPContext->m_AcceptSocket, pWSABUF, 1, &dwBytes, &dwFlags, pOverlapped, NULL);

    //������ش��󣬲��Ҵ���Ĵ��벻��Pending��˵������ʧ��
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

    //1.��ȡ����ͻ��˵ĵ�ַ��Ϣ
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

    //2.����Listen socket�ϵ�Context��Ϊ�������socket����һ���µ�SocketContext
    std::shared_ptr<IOCPSocketContext> pNewIOCPSocketContext = std::make_shared<IOCPSocketContext>();
    pNewIOCPSocketContext->m_Socket = pIOCPContext->m_AcceptSocket;
    memcpy(&(pNewIOCPSocketContext->m_ClientAddr), ClientAddr, sizeof(SOCKADDR_IN));

    //������µ�Socket����ɶ˿ڰ�
    if (!AssociateWithIOCP(pNewIOCPSocketContext.get()))
    {
        return false;
    }

    //3.������Socket�µ�IOContext�����������socket��Ͷ�ݵ�һ��Recv��������
    std::shared_ptr<IOCPContext> pNewIOCPContext = pNewIOCPSocketContext->GetNewIOContext();
    pNewIOCPContext->m_ActionType = IOCP_AT_RECV;
    pNewIOCPContext->m_AcceptSocket = pNewIOCPSocketContext->m_Socket;
    //���Buffer��Ҫ�������Ϳ�������
    //memcpy(pNewIOCPContext->m_szBuffer, pIOCPContext->m_szBuffer, MAX_BUFFER_LEN);

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

bool QingServer::DoRecv(IOCPSocketContext * pSocketContext, IOCPContext *pIOCPContext)
{
    //������Ϣ
    SOCKADDR_IN *ClientAddr = &pSocketContext->m_ClientAddr;
    Qing::QingLog::Write(Qing::LL_INFO, "Recv %s:%d message = %s",
        ClientIP(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_WSABuffer.buf);

    //��ʼͶ����һ��WSARecv����
    return PostRecv(pIOCPContext);
}

bool QingServer::DoSend(IOCPSocketContext * pSocketContext, IOCPContext *pIOCPContext)
{
    return false;
}

bool QingServer::IsSocketAlive(SOCKET socket)
{
    //�жϿͻ���socket�Ƿ��Ѿ��Ͽ���������һ����Чsocket��Ͷ��WSARecv�������׳��쳣
    //�����ǳ��������socket�������ݣ��ж����socket���õķ���ֵ
    //��Ϊ����ͻ��������쳣�Ͽ�(������ε�����)ʱ������޷��յ��ͻ��˶Ͽ���֪ͨ

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
    if (WAIT_TIMEOUT == dwErr)                          //��ʱ
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
    else if (ERROR_NETNAME_DELETED == dwErr)            //�쳣
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

    //ѭ������ֱ���յ�Shutdown��ϢΪֹ
    while (WaitForSingleObject(pQingIOCP->m_hShutdownEvent, 0) != WAIT_OBJECT_0)
    {
        std::shared_ptr<IOCPSocketContext> pSocketContext;

        BOOL bReturn = GetQueuedCompletionStatus(
            pQingIOCP->m_hIOCompletionPort,
            &dwBytesTransfered,
            (PULONG_PTR)&pSocketContext,
            &pOverlapped,
            INFINITE);

        //����յ������˳���Ϣ��ֱ���˳�
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
            //��ȡ����Ĳ���
            IOCPContext *pIOCPContext(CONTAINING_RECORD(pOverlapped, IOCPContext, m_Overlapped));

            //�ж��Ƿ��пͻ��˶Ͽ���
            if ((0 == dwBytesTransfered) && (IOCP_AT_RECV == pIOCPContext->m_ActionType || IOCP_AT_SEND == pIOCPContext->m_ActionType))
            {
                Qing::QingLog::Write(Qing::LL_ERROR, "Client %s:%d disconnected.",
                    ClientIP(&(pSocketContext->m_ClientAddr)).c_str(),
                    ntohs(pSocketContext->m_ClientAddr.sin_port));

                //�ͷ���Դ
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
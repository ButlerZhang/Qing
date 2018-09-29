#include "QingIOCP.h"
#include "..\..\HeaderFiles\QingLog.h"
#include <WS2tcpip.h>



struct WorkerThreadParam
{
    QingIOCP    *m_pQingIOCP;       //���ڵ������еĺ���
    int          m_nThreadNo;       //�̱߳��
};



QingIOCP::QingIOCP() : m_hShutdownEvent(NULL),
                       m_hIOCompletionPort(NULL),
                       m_phWorkerThreads(NULL),
                       m_nThreads(0),
                       m_ServerIP("127.0.0.1"),
                       m_ServerPort(8888),
                       m_pListenContext(NULL),
                       m_lpfnAcceptEx(NULL),
                       m_lpfnGetAcceptExSockAddrs(NULL)
{
}

QingIOCP::~QingIOCP()
{
    Stop();
}

bool QingIOCP::Start()
{
    //��ʼ���߳��ź���
    InitializeCriticalSection(&m_csContextList);

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
    Qing::QingLog::Write("QingIOCP ready.", Qing::LL_INFO);
    return true;
}

void QingIOCP::Stop()
{
    if (m_pListenContext != NULL && m_pListenContext->m_Socket != INVALID_SOCKET)
    {
        //����ر���Ϣ֪ͨ
        SetEvent(m_hShutdownEvent);

        //֪ͨ���е���ɶ˿ڲ����˳�
        for (int i = 0; i < m_nThreads; i++)
        {
            PostQueuedCompletionStatus(m_hIOCompletionPort, 0, NULL, NULL);
        }

        //�ȴ����еĿͻ�����Դ�˳�
        WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);

        //����ͻ����б���Ϣ
        ClearContextList();

        //�ͷ�������Դ
        DeleteInitializeResources();

        Qing::QingLog::Write("Stop listen.", Qing::LL_INFO);
    }
}

std::string QingIOCP::GetLocalIP()
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

    return m_ServerIP;
}

bool QingIOCP::InitializeIOCP()
{
    //������ɶ˿�
    m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == m_hIOCompletionPort)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Create IO completion port error = %d.", WSAGetLastError());
        return false;
    }

    //���ݴ���������������Ӧ���߳���
    m_nThreads = WORKER_THREADS_PER_PROCESSOR * GetProcessorsCount();
    m_phWorkerThreads = new HANDLE[m_nThreads];

    //����worker�߳�
    DWORD nThreadID;
    for (int i = 0; i < m_nThreads; i++)
    {
        WorkerThreadParam *pWorkerThreadParam = new WorkerThreadParam();
        pWorkerThreadParam->m_pQingIOCP = this;
        pWorkerThreadParam->m_nThreadNo = i + 1;
        m_phWorkerThreads[i] = ::CreateThread(0, 0, WorkerThread, (void*)pWorkerThreadParam, 0, &nThreadID);
    }

    Qing::QingLog::Write(Qing::LL_INFO, "Created %d worker threads.", m_nThreads);
    return true;
}

bool QingIOCP::InitializeListenSocket()
{
    //�������ڼ�����socket��Ϣ
    m_pListenContext = new IOCPSocketContext;

    //ʹ��WSASocket����socket���ſ���֧���ص�IO����
    m_pListenContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_pListenContext->m_Socket == INVALID_SOCKET)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Initialize listen socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Initialize listen socket succeed.", Qing::LL_INFO);

    //��listen socket�󶨵���ɶ˿�
    if (CreateIoCompletionPort((HANDLE)m_pListenContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)m_pListenContext, 0) == NULL)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Bind listen socket to IOCP failed, error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Bind listen socket to IOCP succeed.", Qing::LL_INFO);

    //����ַ��Ϣ
    struct sockaddr_in ServerAddress;
    ZeroMemory((char*)&ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);                        //���κ�һ����ַ
    //inet_pton(AF_INET, m_ServerIP.c_str(), &(ServerAddress.sin_addr.s_addr));   //��ĳ��IP��ַ
    ServerAddress.sin_port = htons(m_ServerPort);

    //�󶨵�ַ�Ͷ˿�
    if (bind(m_pListenContext->m_Socket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Listen socket bind failed, error = %d.", WSAGetLastError());
        return false;
    }

    Qing::QingLog::Write("Listen socket bind succeed.", Qing::LL_INFO);

    //��ʼ���м���
    if (listen(m_pListenContext->m_Socket, SOMAXCONN) == SOCKET_ERROR)
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
        m_pListenContext->m_Socket,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidAcceptEx,
        sizeof(GuidAcceptEx),
        &m_lpfnAcceptEx,
        sizeof(m_lpfnAcceptEx),
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
        m_pListenContext->m_Socket,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidGetAcceptExSockAddrs,
        sizeof(GuidGetAcceptExSockAddrs),
        &m_lpfnGetAcceptExSockAddrs,
        sizeof(m_lpfnGetAcceptExSockAddrs),
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
        IOCPContext *pAcceptIOCPContext = m_pListenContext->GetNewIOContext();
        if (!PostAccept(pAcceptIOCPContext))
        {
            m_pListenContext->RemoveContext(pAcceptIOCPContext);
            return false;
        }
    }

    Qing::QingLog::Write(Qing::LL_INFO, "Post %d AcceptEx request.", MAX_POST_ACCEPT);
    return true;
}

void QingIOCP::DeleteInitializeResources()
{
    DeleteCriticalSection(&m_csContextList);
    ReleaseHandle(m_hShutdownEvent);

    for (int i = 0; i < m_nThreads; i++)
    {
        ReleaseHandle(m_phWorkerThreads[i]);
    }

    delete m_phWorkerThreads;
    m_phWorkerThreads = NULL;

    ReleaseHandle(m_hIOCompletionPort);

    delete m_pListenContext;
    m_pListenContext = NULL;

    Qing::QingLog::Write("Delete resources succeed.", Qing::LL_INFO);
}

bool QingIOCP::PostAccept(IOCPContext *pIOCPContext)
{
    DWORD dwBytes = 0;
    pIOCPContext->m_ActionType = IOCP_AT_ACCEPT;
    WSABUF *pWSABUF = &pIOCPContext->m_wsaBuf;
    OVERLAPPED *pOverlapped = &pIOCPContext->m_Overlapped;

    //Ϊ������Ŀͻ�����׼����socket(�����봫ͳaccept��������)
    pIOCPContext->m_sockAccept = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (pIOCPContext->m_sockAccept == INVALID_SOCKET)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Create accept socket failed, error = %d.", WSAGetLastError());
        return false;
    }

    //Ͷ��AcceptEx
    if (!m_lpfnAcceptEx(
        m_pListenContext->m_Socket,
        pIOCPContext->m_sockAccept,
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

bool QingIOCP::PostRecv(IOCPContext * pIOCPContext)
{
    DWORD dwFlags = 0;
    DWORD dwBytes = 0;
    WSABUF *pWSABUF = &pIOCPContext->m_wsaBuf;
    OVERLAPPED *pOverlapped = &pIOCPContext->m_Overlapped;

    pIOCPContext->ResetBuffer();
    pIOCPContext->m_ActionType = IOCP_AT_RECV;

    //Ͷ��WSARecv����
    int nBytesRecv = WSARecv(pIOCPContext->m_sockAccept, pWSABUF, 1, &dwBytes, &dwFlags, pOverlapped, NULL);

    //������ش��󣬲��Ҵ���Ĵ��벻��Pending��˵������ʧ��
    if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Post recv failed, error = %d.", WSAGetLastError());
        return false;
    }

    return true;
}

bool QingIOCP::PostSend(IOCPContext * pIOCPContext)
{
    Qing::QingLog::Write("Post send succeed.");
    return true;
}

bool QingIOCP::DoAccept(IOCPSocketContext * pSocketContext, IOCPContext * pIOCPContext)
{
    SOCKADDR_IN *ClientAddr = NULL;
    SOCKADDR_IN *LocalAddr = NULL;
    int RemoteLen = sizeof(SOCKADDR_IN);
    int LocalLen = sizeof(SOCKADDR_IN);

    //1.��ȡ����ͻ��˵ĵ�ַ��Ϣ
    m_lpfnGetAcceptExSockAddrs(
        pIOCPContext->m_wsaBuf.buf,
        pIOCPContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN) + 16) * 2),
        sizeof(SOCKADDR_IN) + 16,
        sizeof(SOCKADDR_IN) + 16,
        (LPSOCKADDR*)&LocalAddr,
        &LocalLen,
        (LPSOCKADDR*)&ClientAddr,
        &RemoteLen);

    Qing::QingLog::Write(Qing::LL_INFO, "Client %s:%d connected, message = %s.",
        ClientIP(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_wsaBuf.buf);

    //2.����Listen socket�ϵ�Context��Ϊ�������socket����һ���µ�SocketContext
    IOCPSocketContext *pNewIOCPSocketContext = new IOCPSocketContext();
    pNewIOCPSocketContext->m_Socket = pIOCPContext->m_sockAccept;
    memcpy(&(pNewIOCPSocketContext->m_ClientAddr), ClientAddr, sizeof(SOCKADDR_IN));

    //������µ�Socket����ɶ˿ڰ�
    if (!AssociateWithIOCP(pNewIOCPSocketContext))
    {
        delete pNewIOCPSocketContext;
        pNewIOCPSocketContext = NULL;
        return false;
    }

    //3.������Socket�µ�IOContext�����������socket��Ͷ�ݵ�һ��Recv��������
    IOCPContext *pNewIOCPContext = pNewIOCPSocketContext->GetNewIOContext();
    pNewIOCPContext->m_ActionType = IOCP_AT_RECV;
    pNewIOCPContext->m_sockAccept = pNewIOCPSocketContext->m_Socket;
    //���Buffer��Ҫ�������Ϳ�������
    //memcpy(pNewIOCPContext->m_szBuffer, pIOCPContext->m_szBuffer, MAX_BUFFER_LEN);

    //�����֮�󣬾Ϳ��Կ�ʼ�����socket��Ͷ�����������
    if (!PostRecv(pNewIOCPContext))
    {
        pNewIOCPSocketContext->RemoveContext(pNewIOCPContext);
        return false;
    }

    //4.���Ͷ�ݳɹ������¼�����Ч�Ŀͻ�����Ϣ
    AddToContextList(pNewIOCPSocketContext);

    //5.����listen socket��׼��Ͷ���µ�AcceptEx
    pIOCPContext->ResetBuffer();
    return PostAccept(pIOCPContext);
}

bool QingIOCP::DoRecv(IOCPSocketContext * pSocketContext, IOCPContext * pIOCPContext)
{
    //������Ϣ
    SOCKADDR_IN *ClientAddr = &pSocketContext->m_ClientAddr;
    Qing::QingLog::Write(Qing::LL_INFO, "Recv %s:%d message = %s",
        ClientIP(ClientAddr).c_str(),
        ntohs(ClientAddr->sin_port),
        pIOCPContext->m_wsaBuf.buf);

    //��ʼͶ����һ��WSARecv����
    return PostRecv(pIOCPContext);
}

bool QingIOCP::DoSend(IOCPSocketContext * pSocketContext, IOCPContext * pIOCPContext)
{
    return false;
}

void QingIOCP::AddToContextList(IOCPSocketContext * pSocketContext)
{
    EnterCriticalSection(&m_csContextList);
    m_ClientContextVector.push_back(pSocketContext);
    LeaveCriticalSection(&m_csContextList);
}

void QingIOCP::RemoveContextList(IOCPSocketContext * pSocketContext)
{
    EnterCriticalSection(&m_csContextList);
    for (auto Index = 0; Index < m_ClientContextVector.size(); Index++)
    {
        if (pSocketContext == m_ClientContextVector[Index])
        {
            m_ClientContextVector.erase(m_ClientContextVector.begin() + Index);
            delete pSocketContext;
            pSocketContext = NULL;
            break;
        }
    }
    LeaveCriticalSection(&m_csContextList);
}

void QingIOCP::ClearContextList()
{
    EnterCriticalSection(&m_csContextList);
    for (auto Index = 0; Index < m_ClientContextVector.size(); Index++)
    {
        delete m_ClientContextVector[Index];
        m_ClientContextVector[Index] = NULL;
    }
    m_ClientContextVector.clear();
    LeaveCriticalSection(&m_csContextList);
}

int QingIOCP::GetProcessorsCount()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
}

bool QingIOCP::IsSocketAlive(SOCKET socket)
{
    //�жϿͻ���socket�Ƿ��Ѿ��Ͽ���������һ����Чsocket��Ͷ��WSARecv�������׳��쳣
    //�����ǳ��������socket�������ݣ��ж����socket���õķ���ֵ
    //��Ϊ����ͻ��������쳣�Ͽ�(������ε�����)ʱ������޷��յ��ͻ��˶Ͽ���֪ͨ

    int nByteSend = send(socket, "", 0, 0);
    return (nByteSend != -1);
}

void QingIOCP::ReleaseHandle(HANDLE Handle)
{
    if (Handle != NULL && Handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(Handle);
        Handle = NULL;
    }
}

bool QingIOCP::AssociateWithIOCP(IOCPSocketContext * pSocketContext)
{
    HANDLE Handle = CreateIoCompletionPort((HANDLE)pSocketContext->m_Socket, m_hIOCompletionPort, (ULONG_PTR)pSocketContext, 0);
    if (Handle == NULL)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Associate with IOCP error = %d.", GetLastError());
        return false;
    }

    return true;
}

bool QingIOCP::HandleError(IOCPSocketContext * pSocketContext, const DWORD & dwErr)
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
            RemoveContextList(pSocketContext);
            return true;
        }
    }
    else if (ERROR_NETNAME_DELETED == dwErr)            //�쳣
    {
        Qing::QingLog::Write("Client disconnected.", Qing::LL_INFO);
        RemoveContextList(pSocketContext);
        return true;
    }
    else
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "IOCP error = %d, exit thread.", dwErr);
        return false;
    }
}

std::string QingIOCP::ClientIP(SOCKADDR_IN *ClientAddr)
{
    char TempIPArray[INET_ADDRSTRLEN];
    memset(TempIPArray, 0, sizeof(TempIPArray));

    inet_ntop(AF_INET, &(ClientAddr->sin_addr), TempIPArray, sizeof(TempIPArray));
    return std::string(TempIPArray);
}

DWORD QingIOCP::WorkerThread(LPVOID lpParam)
{
    WorkerThreadParam *pParam = (WorkerThreadParam*)lpParam;
    QingIOCP *pQingIOCP = (QingIOCP*)pParam->m_pQingIOCP;
    int nThreadNo = (int)pParam->m_nThreadNo;
    Qing::QingLog::Write(Qing::LL_INFO, "Worker thread ID = %d start.", nThreadNo);

    OVERLAPPED *pOverlapped = NULL;
    IOCPSocketContext *pSocketContext = NULL;
    DWORD dwBytesTransfered = 0;

    //ѭ������ֱ���յ�Shutdown��ϢΪֹ
    while (WaitForSingleObject(pQingIOCP->m_hShutdownEvent, 0) != WAIT_OBJECT_0)
    {
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
            if (!pQingIOCP->HandleError(pSocketContext, dwErr))
            {
                break;
            }

            continue;
        }
        else
        {
            //��ȡ����Ĳ���
            IOCPContext *pIOCPContext = CONTAINING_RECORD(pOverlapped, IOCPContext, m_Overlapped);

            //�ж��Ƿ��пͻ��˶Ͽ���
            if ((0 == dwBytesTransfered) && (IOCP_AT_RECV == pIOCPContext->m_ActionType || IOCP_AT_SEND == pIOCPContext->m_ActionType))
            {
                Qing::QingLog::Write(Qing::LL_ERROR, "Client %s:%d disconnected.",
                    ClientIP(&(pSocketContext->m_ClientAddr)).c_str(),
                    ntohs(pSocketContext->m_ClientAddr.sin_port));

                //�ͷ���Դ
                pQingIOCP->RemoveContextList(pSocketContext);
                continue;
            }
            else
            {
                switch (pIOCPContext->m_ActionType)
                {
                case IOCP_AT_ACCEPT:    pQingIOCP->DoAccept(pSocketContext, pIOCPContext);              break;
                case IOCP_AT_RECV:      pQingIOCP->DoRecv(pSocketContext, pIOCPContext);                break;
                case IOCP_AT_SEND:      pQingIOCP->DoSend(pSocketContext, pIOCPContext);                break;
                default:                Qing::QingLog::Write("Worker thread action type error");        break;
                }
            }
        }
    }

    Qing::QingLog::Write(Qing::LL_INFO, "Worker thread ID = %d exit.", nThreadNo);
    delete lpParam;
    lpParam = NULL;

    return 0;
}

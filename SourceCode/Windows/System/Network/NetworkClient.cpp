#include "NetworkClient.h"
#include "NetworkEnvironment.h"
#include "../../../ProjectLinux/Share/Boost/BoostLog.h"
#include "../LocalComputer.h"



NetworkClient::NetworkClient() : NetworkBase(),
                                 m_IsConnected(false)
{
    m_SocketContext = std::make_shared<IOCPSocketContext>();
}

NetworkClient::~NetworkClient()
{
    Stop();
}

bool NetworkClient::Start(const std::string &ServerIP, int Port)
{
    if (NetworkBase::Start(ServerIP, Port))
    {
        if (CreateWorkerThread(1) && CreateSocket())
        {
            ConnectServer(ServerIP, Port);
            g_Log.WriteInfo("Start succeed, NetworkClient ready.");
            return true;
        }
    }

    return false;
}

void NetworkClient::Stop()
{
    if (m_SocketContext->m_Socket != INVALID_SOCKET)
    {
        NetworkBase::Stop();
        ReleaseSocket(m_SocketContext->m_Socket);
        m_SocketContext->m_Socket = INVALID_SOCKET;
        g_Log.WriteInfo("Stop client.");
    }
}

int NetworkClient::Send(const void * MessageData, int MessageSize)
{
    std::shared_ptr<IOCPContext> SendIOCPContext = m_SocketContext->GetNewIOContext(GlobalNetwork.GetNextTrackID());

    SendIOCPContext->m_AcceptSocket = m_SocketContext->m_Socket;
    SendIOCPContext->m_WSABuffer.len = MessageSize;

    memcpy(SendIOCPContext->m_Buffer, MessageData, MessageSize);
    memcpy(SendIOCPContext->m_WSABuffer.buf, MessageData, MessageSize);

    return PostSend(*SendIOCPContext) ? MessageSize : 0;
}

void NetworkClient::WorkerThread()
{
    OVERLAPPED *pOverlapped = NULL;
    DWORD dwBytesTransfered = 0;

    //ѭ������ֱ���յ�Shutdown��ϢΪֹ
    while (WaitForSingleObject(m_hWorkerThreadExitEvent, 0) != WAIT_OBJECT_0)
    {
        BOOL bReturn = GetQueuedCompletionStatus(
            GlobalNetwork.GetIOCP(),                        //��ɶ˿�
            &dwBytesTransfered,                             //������ɺ󷵻ص��ֽ���
            (PULONG_PTR)&(m_SocketContext->m_Socket),        //������ɶ˿�ʱ�󶨵��Զ���ṹ�����
            &pOverlapped,                                   //����socketʱһ�������ص��ṹ
            INFINITE);                                      //�ȴ���ɶ˿ڵĳ�ʱʱ�䣬����̲߳���Ҫ����������������ΪINFINITE

        if (!bReturn)
        {
            continue;
        }

        if (pOverlapped == NULL)
        {
            break;
        }

        //��ȡ����Ĳ���
        IOCPContext *pIOCPContext(CONTAINING_RECORD(pOverlapped, IOCPContext, m_Overlapped));

        //�Ƿ����
        if ((0 == dwBytesTransfered) && (IOCP_AT_RECV == pIOCPContext->m_ActionType || IOCP_AT_SEND == pIOCPContext->m_ActionType))
        {
            //TODO
            continue;
        }

        switch (pIOCPContext->m_ActionType)
        {
        case IOCP_AT_RECV:      ProcessRecv(*pIOCPContext);                                 break;
        case IOCP_AT_SEND:      ProcessSend(*pIOCPContext);                                 break;
        default:                g_Log.WriteError("Worker thread action type error");        break;
        }
    }
}

bool NetworkClient::CreateSocket()
{
    m_SocketContext->m_Socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
    if (m_SocketContext->m_Socket == INVALID_SOCKET)
    {
        g_Log.WriteError(BoostFormat("Create client socket fail, error = %d.", WSAGetLastError()));
        return false;
    }

    g_Log.WriteInfo(BoostFormat("Create client Socket = %I64d succeed.", m_SocketContext->m_Socket));

    if (CreateIoCompletionPort((HANDLE)(m_SocketContext->m_Socket), GlobalNetwork.GetIOCP(), m_SocketContext->m_Socket, 0) == NULL)
    {
        g_Log.WriteError(BoostFormat("Client socket associate with IOCP error = %d.", GetLastError()));
        return false;
    }

    return true;
}

unsigned __int64 NetworkClient::GetClientID() const
{
    if (m_SocketContext != NULL)
    {
        return m_SocketContext->m_Socket;
    }

    return 0;
}

bool NetworkClient::ConnectServer(const std::string & ServerIP, int Port)
{
    if (m_SocketContext->m_Socket == INVALID_SOCKET)
    {
        g_Log.WriteError("Invalid Socket.");
        return false;
    }

    if (m_IsConnected)
    {
        return false;
    }

    struct sockaddr_in ServerAddress;
    FillAddress(ServerAddress);

    if (::WSAConnect(m_SocketContext->m_Socket, reinterpret_cast<const struct sockaddr*>(&ServerAddress), sizeof(ServerAddress),0,0,0,0) == SOCKET_ERROR)
    {
        g_Log.WriteError(BoostFormat("Connect server failed, error = %d.", WSAGetLastError()));
        return false;
    }

    m_IsConnected = true;
    SetSocketLinger(m_SocketContext->m_Socket);

    ReadyToRecvData();
    return true;
}

void NetworkClient::ReadyToRecvData()
{
    std::string LoginMessage("Login Message");
    Send(LoginMessage.c_str(), static_cast<int>(LoginMessage.size()));

    m_SocketContext->m_RecvIOCPContext = std::make_shared<IOCPContext>(GlobalNetwork.GetNextTrackID());
    m_SocketContext->m_RecvIOCPContext->m_AcceptSocket = m_SocketContext->m_Socket;
    PostRecv(*(m_SocketContext->m_RecvIOCPContext));
}

bool NetworkClient::ProcessRecv(IOCPContext &RecvIOCPContext)
{
    g_Log.WriteInfo(BoostFormat("Recv message = %s", RecvIOCPContext.m_WSABuffer.buf));
    return PostRecv(RecvIOCPContext);
}

bool NetworkClient::ProcessSend(IOCPContext &SendIOCPContext)
{
    g_Log.WriteInfo(BoostFormat("Send message = %s", SendIOCPContext.m_WSABuffer.buf));
    m_SocketContext->DeleteContext(SendIOCPContext);
    return true;
}

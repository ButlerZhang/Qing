#include "NetworkClient.h"
#include "NetworkEnvironment.h"
#include "..\..\Boost\BoostLog.h"
#include "..\..\Boost\BoostFormat.h"
#include "..\LocalComputer.h"

QING_NAMESPACE_BEGIN



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
            BoostLog::WriteInfo(L"Start succeed, NetworkClient ready.");
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
        BoostLog::WriteInfo(L"Stop client.");
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

    //循环处理，直到收到Shutdown消息为止
    while (WaitForSingleObject(m_hWorkerThreadExitEvent, 0) != WAIT_OBJECT_0)
    {
        BOOL bReturn = GetQueuedCompletionStatus(
            GlobalNetwork.GetIOCP(),                        //完成端口
            &dwBytesTransfered,                             //操作完成后返回的字节数
            (PULONG_PTR)&(m_SocketContext->m_Socket),        //建立完成端口时绑定的自定义结构体参数
            &pOverlapped,                                   //连入socket时一起建立的重叠结构
            INFINITE);                                      //等待完成端口的超时时间，如果线程不需要做其它事情则设置为INFINITE

        if (!bReturn)
        {
            continue;
        }

        if (pOverlapped == NULL)
        {
            break;
        }

        //读取传入的参数
        IOCPContext *pIOCPContext(CONTAINING_RECORD(pOverlapped, IOCPContext, m_Overlapped));

        //是否断连
        if ((0 == dwBytesTransfered) && (IOCP_AT_RECV == pIOCPContext->m_ActionType || IOCP_AT_SEND == pIOCPContext->m_ActionType))
        {
            //TODO
            continue;
        }

        switch (pIOCPContext->m_ActionType)
        {
        case IOCP_AT_RECV:      ProcessRecv(*pIOCPContext);                                 break;
        case IOCP_AT_SEND:      ProcessSend(*pIOCPContext);                                 break;
        default:                BoostLog::WriteError(L"Worker thread action type error");   break;
        }
    }
}

bool NetworkClient::CreateSocket()
{
    m_SocketContext->m_Socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
    if (m_SocketContext->m_Socket == INVALID_SOCKET)
    {
        BoostLog::WriteError(BoostFormat(L"Create client socket fail, error = %d.", WSAGetLastError()));
        return false;
    }

    BoostLog::WriteInfo(BoostFormat(L"Create client Socket = %I64d succeed.", m_SocketContext->m_Socket));

    if (CreateIoCompletionPort((HANDLE)(m_SocketContext->m_Socket), GlobalNetwork.GetIOCP(), m_SocketContext->m_Socket, 0) == NULL)
    {
        BoostLog::WriteError(BoostFormat(L"Client socket associate with IOCP error = %d.", GetLastError()));
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
        BoostLog::WriteError(L"Invalid Socket.");
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
        BoostLog::WriteError(BoostFormat(L"Connect server failed, error = %d.", WSAGetLastError()));
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
    BoostLog::WriteInfo(BoostFormat(L"Recv message = %s", RecvIOCPContext.m_WSABuffer.buf));
    return PostRecv(RecvIOCPContext);
}

bool NetworkClient::ProcessSend(IOCPContext &SendIOCPContext)
{
    BoostLog::WriteInfo(BoostFormat(L"Send message = %s", SendIOCPContext.m_WSABuffer.buf));
    m_SocketContext->DeleteContext(SendIOCPContext);
    return true;
}

QING_NAMESPACE_END
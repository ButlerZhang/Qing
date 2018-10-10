#include "NetworkClient.h"
#include "..\..\HeaderFiles\QingLog.h"
#include "..\..\HeaderFiles\LocalComputer.h"

QING_NAMESPACE_BEGIN



NetworkClient::NetworkClient() : NetworkBase(),
                                 m_IsConnected(false),
                                 m_ClientSocket(INVALID_SOCKET)
{
}

NetworkClient::~NetworkClient()
{
    Stop();
}

bool NetworkClient::Start(const std::string &ServerIP, int Port)
{
    if (NetworkBase::Start(ServerIP, Port))
    {
        if (CreateIOCP() &&
            CreateWorkerThread(1) &&
            CreateSocket() &&
            ConnectServer(ServerIP, Port))
        {
            m_IOCPContext = std::make_shared<IOCPContext>();
            QingLog::Write("Start succeed, NetworkClient ready.", LL_INFO);
            return true;
        }
    }

    return false;
}

void NetworkClient::Stop()
{
    NetworkBase::Stop();
    QingLog::Write("Stop client.", LL_INFO);
}

int NetworkClient::Send(const void * MessageData, int MessageSize)
{
    //std::shared_ptr<IOCPContext> NewContext = std::make_shared<IOCPContext>();
    //m_IOContextVector.push_back(NewContext);

    m_IOCPContext->ResetBuffer();
    m_IOCPContext->m_AcceptSocket = m_ClientSocket;
    m_IOCPContext->m_ActionType = IOCP_AT_SEND;
    m_IOCPContext->m_WSABuffer.len = MessageSize;
    memcpy(m_IOCPContext->m_Buffer, MessageData, MessageSize);
    memcpy(m_IOCPContext->m_WSABuffer.buf, MessageData, MessageSize);

    PostSend(m_IOCPContext.get());
    return MessageSize;
}

void NetworkClient::WorkerThread()
{
    OVERLAPPED *pOverlapped = NULL;
    DWORD dwBytesTransfered = 0;

    //循环处理，直到收到Shutdown消息为止
    while (WaitForSingleObject(m_hWorkerThreadExitEvent, 0) != WAIT_OBJECT_0)
    {
        BOOL bReturn = GetQueuedCompletionStatus(
            m_hIOCompletionPort,                //完成端口
            &dwBytesTransfered,                 //操作完成后返回的字节数
            (PULONG_PTR)&m_ClientSocket,        //建立完成端口时绑定的自定义结构体参数
            &pOverlapped,                       //连入socket时一起建立的重叠结构
            INFINITE);                          //等待完成端口的超时时间，如果线程不需要做其它事情则设置为INFINITE

        if (!bReturn)
        {
            //if (!pQingIOCP->HandleError(pSocketContext, GetLastError()))
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
                //LocalComputer MyComputer;
                //QingLog::Write(LL_ERROR, "Client %s:%d disconnected.",
                //    MyComputer.ConvertToIPString(&(pSocketContext->m_ClientAddr)).c_str(),
                //    ntohs(pSocketContext->m_ClientAddr.sin_port));

                //释放资源
                //pQingIOCP->m_ClientManager.RemoveClient(pSocketContext);
                continue;
            }
            else
            {
                switch (pIOCPContext->m_ActionType)
                {
                case IOCP_AT_RECV:      ProcessRecv(pIOCPContext);                   break;
                case IOCP_AT_SEND:      ProcessSend(pIOCPContext);                   break;
                default:                QingLog::Write("Worker thread action type error");      break;
                }
            }
        }
    }
}

bool NetworkClient::CreateSocket()
{
    m_ClientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
    if (m_ClientSocket == INVALID_SOCKET)
    {
        QingLog::Write(LL_ERROR, "Create client socket fail, error = %d.", WSAGetLastError());
        return false;
    }

    QingLog::Write(LL_INFO, "Create client socket = %d succeed.", m_ClientSocket);

    if (CreateIoCompletionPort((HANDLE)m_ClientSocket, m_hIOCompletionPort, m_ClientSocket, 0) == NULL)
    {
        QingLog::Write(LL_ERROR, "Client socket associate with IOCP error = %d.", GetLastError());
        return false;
    }

    return true;
}

bool NetworkClient::ConnectServer(const std::string & ServerIP, int Port)
{
    if (m_ClientSocket == INVALID_SOCKET)
    {
        return false;
    }

    if (m_IsConnected)
    {
        return false;
    }

    struct sockaddr_in ServerAddress;
    FillAddress(ServerAddress);

    if (::WSAConnect(m_ClientSocket, reinterpret_cast<const struct sockaddr*>(&ServerAddress), sizeof(ServerAddress),0,0,0,0) == SOCKET_ERROR)
    {
        QingLog::Write(LL_ERROR, "Connect server failed, error = %d.", WSAGetLastError());
        return false;
    }

    m_IsConnected = true;
    SetSocketLinger(m_ClientSocket);

    //m_IOContextVector.push_back(std::make_shared<IOCPContext>());
    //PostRecv(m_IOContextVector[0].get());

    return true;
}

bool NetworkClient::PostRecv(IOCPContext *pIOCPContext)
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

bool NetworkClient::PostSend(IOCPContext *pIOCPContext)
{
    //pIOCPContext->ResetBuffer();
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

bool NetworkClient::ProcessRecv(IOCPContext * pIOCPContext)
{
    QingLog::Write(LL_INFO, "Recv message = %s", pIOCPContext->m_WSABuffer.buf);
    QingLog::Write(LL_INFO, "Recv message = %s", pIOCPContext->m_Buffer);
    return PostRecv(pIOCPContext);
}

bool NetworkClient::ProcessSend(IOCPContext * pIOCPContext)
{
    QingLog::Write(LL_INFO, "Send message = %s", pIOCPContext->m_WSABuffer.buf);
    QingLog::Write(LL_INFO, "Send message = %s", pIOCPContext->m_Buffer);
    return true;
}

QING_NAMESPACE_END
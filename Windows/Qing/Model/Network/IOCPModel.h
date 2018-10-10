#pragma once
#include "..\..\HeaderFiles\QingBase.h"
#include <winsock2.h>
#include <MSWSock.h>
#include <memory>
#include <vector>

QING_NAMESPACE_BEGIN



enum IOCPEnum
{
    MAX_WORKER_THREAD_COUNT = 100,                       //最多可以有100条工作线程
    MAX_IO_CONTEXT_BUFFER_LEN = 1024,                    //通常情况下MTU是1500,所以设为1k
    WORKER_THREADS_PER_PROCESSOR = 2,                    //工作线程数是CPU核心数的两倍(MSDN建议两倍)
    ACCEPTEX_ADDRESS_LENGTH = sizeof(SOCKADDR_IN) + 16,  //AcceptEx参数地址长度,16是微软规定的
};



enum IOCPActionType
{
    IOCP_AT_NULL,
    IOCP_AT_ACCEPT,
    IOCP_AT_SEND,
    IOCP_AT_RECV,
};



struct IOCPContext
{
    OVERLAPPED          m_Overlapped;                               //每个IO操作的重叠结构
    SOCKET              m_AcceptSocket;                             //当前网络操作所使用的socket
    WSABUF              m_WSABuffer;                                //缓冲区，用于给重叠操作传参数的
    IOCPActionType      m_ActionType;                               //要执行的网络操作
    char                m_Buffer[MAX_IO_CONTEXT_BUFFER_LEN];        //具体数据的缓冲区,对应WSABUF里的缓冲区

    IOCPContext()
    {
        ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
        ZeroMemory(&m_Buffer, sizeof(m_Buffer));
        m_AcceptSocket = INVALID_SOCKET;
        m_WSABuffer.buf = m_Buffer;
        m_WSABuffer.len = MAX_IO_CONTEXT_BUFFER_LEN;
        m_ActionType = IOCP_AT_NULL;
    }

    ~IOCPContext()
    {
        if (m_AcceptSocket != INVALID_SOCKET)
        {
            closesocket(m_AcceptSocket);
            m_AcceptSocket = INVALID_SOCKET;
        }
    }

    void ResetBuffer()
    {
        ZeroMemory(&m_Buffer, sizeof(m_Buffer));
    }
};



struct IOCPSocketContext
{
    SOCKET                                      m_Socket;           //每个客户端连接的socket
    SOCKADDR_IN                                 m_ClientAddr;       //客户端的地址
    std::vector<std::shared_ptr<IOCPContext>>   m_IOContextVector;  //客户端IO操作的上下文数据

    IOCPSocketContext()
    {
        m_Socket = INVALID_SOCKET;
        ZeroMemory(&m_ClientAddr, sizeof(m_ClientAddr));
    }

    ~IOCPSocketContext()
    {
        if (m_Socket != INVALID_SOCKET)
        {
            closesocket(m_Socket);
            m_Socket = INVALID_SOCKET;
        }

        m_IOContextVector.clear();
    }

    std::shared_ptr<IOCPContext> GetNewIOContext()
    {
        std::shared_ptr<IOCPContext> NewIOContext = std::make_shared<IOCPContext>();
        m_IOContextVector.push_back(NewIOContext);
        return NewIOContext;
    }

    bool RemoveContext(const std::shared_ptr<IOCPContext> &RemoveIOContext)
    {
        for (auto Index = 0; Index < m_IOContextVector.size(); Index++)
        {
            if (RemoveIOContext == m_IOContextVector[Index])
            {
                m_IOContextVector.erase(m_IOContextVector.begin() + Index);
                break;
            }
        }

        return false;
    }
};



struct WorkerThreadParam
{
    unsigned long   m_ThreadID;
    int             m_ThreadIndex;
    void*           m_Network;
};



QING_NAMESPACE_END
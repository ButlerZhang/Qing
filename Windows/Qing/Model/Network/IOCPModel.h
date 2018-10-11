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



void ReleaseSocket(SOCKET &Socket);



struct IOCPContext
{
    OVERLAPPED          m_Overlapped;                               //每个IO操作的重叠结构
    SOCKET              m_AcceptSocket;                             //当前网络操作所使用的socket
    WSABUF              m_WSABuffer;                                //缓冲区，用于给重叠操作传参数的
    IOCPActionType      m_ActionType;                               //要执行的网络操作
    unsigned __int64    m_ContextID;                                //ID
    char                m_Buffer[MAX_IO_CONTEXT_BUFFER_LEN];        //具体数据的缓冲区,对应WSABUF里的缓冲区

    IOCPContext(unsigned __int64 ID);
    ~IOCPContext();
    void ResetBuffer();
};



struct IOCPSocketContext
{
    SOCKET                                      m_Socket;           //每个客户端连接的socket
    SOCKADDR_IN                                 m_ClientAddr;       //客户端的地址
    std::shared_ptr<IOCPContext>                m_SendIOCPContext;  //
    std::vector<std::shared_ptr<IOCPContext>>   m_IOContextVector;  //客户端IO操作的上下文数据

    IOCPSocketContext();
    ~IOCPSocketContext();

    std::shared_ptr<IOCPContext> GetNewIOContext(unsigned __int64 ID);
    bool RemoveContext(const std::shared_ptr<IOCPContext> &RemoveIOContext);
};



struct WorkerThreadParam
{
    unsigned long   m_ThreadID;
    int             m_ThreadIndex;
    void*           m_Network;
};

QING_NAMESPACE_END
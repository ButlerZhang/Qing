#pragma once
#include <winsock2.h>
#include <MSWSock.h>
#include <vector>



enum IOCPEnum
{
    MAX_BUFFER_LEN = 8192,                               //经验值
    WORKER_THREADS_PER_PROCESSOR = 2,                    //工作线程数是CPU核心数的两倍
    MAX_POST_ACCEPT = 10,                                //同时投递的Accept请求的数量
};



enum IOCPActionType
{
    IOCP_AT_ACCEPT,
    IOCP_AT_SEND,
    IOCP_AT_RECV,
    IOCP_AT_SHUTDOWN,
};



struct IOCPContext
{
    OVERLAPPED          m_Overlapped;                    //每个IO操作的重叠结构
    SOCKET              m_sockAccept;                    //当前网络操作所使用的socket
    WSABUF              m_wsaBuf;                        //缓冲区，用于给重叠操作传参数的
    char                m_szBuffer[MAX_BUFFER_LEN];      //具体字符的缓冲区
    IOCPActionType      m_ActionType;                    //要执行的网络操作

    IOCPContext()
    {
        ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
        ZeroMemory(&m_szBuffer, sizeof(m_szBuffer));
        m_sockAccept = INVALID_SOCKET;
        m_wsaBuf.buf = m_szBuffer;
        m_wsaBuf.len = MAX_BUFFER_LEN;
        m_ActionType = IOCP_AT_ACCEPT;
    }

    ~IOCPContext()
    {
        if (m_sockAccept != INVALID_SOCKET)
        {
            closesocket(m_sockAccept);
            m_sockAccept = INVALID_SOCKET;
        }
    }

    void ResetBuffer()
    {
        ZeroMemory(&m_szBuffer, sizeof(m_szBuffer));
    }
};



struct IOCPSocketContext
{
    SOCKET              m_Socket;                        //每个客户端连接的socket
    SOCKADDR_IN         m_ClientAddr;                    //客户端的地址
    std::vector<IOCPContext*>   m_IOContextVector;       //客户端IO操作的上下文数据

    IOCPSocketContext()
    {
        m_Socket = INVALID_SOCKET;
        memset(&m_ClientAddr, 0, sizeof(m_ClientAddr));
    }

    ~IOCPSocketContext()
    {
        if (m_Socket != INVALID_SOCKET)
        {
            closesocket(m_Socket);
            m_Socket = INVALID_SOCKET;
        }

        for (auto Index = 0; Index < m_IOContextVector.size(); Index++)
        {
            delete m_IOContextVector[Index];
        }

        m_IOContextVector.clear();
    }

    IOCPContext* GetNewIOContext()
    {
        IOCPContext *pIOCP = new IOCPContext;
        m_IOContextVector.push_back(pIOCP);
        return pIOCP;
    }

    bool RemoveContext(IOCPContext *pContext)
    {
        if (pContext == NULL)
        {
            return false;
        }

        for (auto Index = 0; Index < m_IOContextVector.size(); Index++)
        {
            if (pContext == m_IOContextVector[Index])
            {
                m_IOContextVector.erase(m_IOContextVector.begin() + Index);
                delete pContext;
                pContext = NULL;
                return true;
            }
        }

        return false;
    }
};

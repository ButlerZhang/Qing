#pragma once
#include <winsock2.h>
#include <MSWSock.h>
#include <vector>



enum IOCPEnum
{
    MAX_BUFFER_LEN = 8192,                               //����ֵ
    WORKER_THREADS_PER_PROCESSOR = 2,                    //�����߳�����CPU������������
    MAX_POST_ACCEPT = 10,                                //ͬʱͶ�ݵ�Accept���������
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
    OVERLAPPED          m_Overlapped;                    //ÿ��IO�������ص��ṹ
    SOCKET              m_sockAccept;                    //��ǰ���������ʹ�õ�socket
    WSABUF              m_wsaBuf;                        //�����������ڸ��ص�������������
    char                m_szBuffer[MAX_BUFFER_LEN];      //�����ַ��Ļ�����
    IOCPActionType      m_ActionType;                    //Ҫִ�е��������

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
    SOCKET              m_Socket;                        //ÿ���ͻ������ӵ�socket
    SOCKADDR_IN         m_ClientAddr;                    //�ͻ��˵ĵ�ַ
    std::vector<IOCPContext*>   m_IOContextVector;       //�ͻ���IO����������������

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

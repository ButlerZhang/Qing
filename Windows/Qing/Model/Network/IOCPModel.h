#pragma once
#include "..\..\HeaderFiles\QingBase.h"
#include <winsock2.h>
#include <MSWSock.h>
#include <memory>
#include <vector>

QING_NAMESPACE_BEGIN



enum IOCPEnum
{
    MAX_WORKER_THREAD_COUNT = 100,                       //��������100�������߳�
    MAX_IO_CONTEXT_BUFFER_LEN = 1024,                    //ͨ�������MTU��1500,������Ϊ1k
    WORKER_THREADS_PER_PROCESSOR = 2,                    //�����߳�����CPU������������(MSDN��������)
    ACCEPTEX_ADDRESS_LENGTH = sizeof(SOCKADDR_IN) + 16,  //AcceptEx������ַ����,16��΢��涨��
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
    OVERLAPPED          m_Overlapped;                               //ÿ��IO�������ص��ṹ
    SOCKET              m_AcceptSocket;                             //��ǰ���������ʹ�õ�socket
    WSABUF              m_WSABuffer;                                //�����������ڸ��ص�������������
    IOCPActionType      m_ActionType;                               //Ҫִ�е��������
    char                m_Buffer[MAX_IO_CONTEXT_BUFFER_LEN];        //�������ݵĻ�����,��ӦWSABUF��Ļ�����

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
    SOCKET                                      m_Socket;           //ÿ���ͻ������ӵ�socket
    SOCKADDR_IN                                 m_ClientAddr;       //�ͻ��˵ĵ�ַ
    std::vector<std::shared_ptr<IOCPContext>>   m_IOContextVector;  //�ͻ���IO����������������

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
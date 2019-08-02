#pragma once
#include <winsock2.h>
#include <MSWSock.h>
#include <memory>
#include <queue>



enum IOCPEnum
{
    MAX_WORKER_THREAD_COUNT = 100,                       //��������100�������߳�
    MAX_IO_CONTEXT_BUFFER_LEN = 1024,                    //ͨ�������MTU��1500,������Ϊ1k
    WORKER_THREADS_PER_PROCESSOR = 2,                    //�����߳�����CPU������������(MSDN��������)
    ACCEPTEX_ADDRESS_LENGTH = sizeof(SOCKADDR_IN) + 16,  //AcceptEx������ַ����,16��΢���涨��
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
    OVERLAPPED          m_Overlapped;                               //ÿ��IO�������ص��ṹ
    SOCKET              m_AcceptSocket;                             //��ǰ���������ʹ�õ�socket
    WSABUF              m_WSABuffer;                                //�����������ڸ��ص�������������
    IOCPActionType      m_ActionType;                               //Ҫִ�е��������
    unsigned __int64    m_ContextID;                                //track ID
    char                m_Buffer[MAX_IO_CONTEXT_BUFFER_LEN];        //�������ݵĻ�����,��ӦWSABUF��Ļ�����

    IOCPContext(unsigned __int64 ID);
    ~IOCPContext();
    void ResetBuffer();
};



struct IOCPSocketContext
{
    SOCKET                                      m_Socket;
    SOCKADDR_IN                                 m_RemoteAddr;
    CRITICAL_SECTION                            m_QueueSection;
    std::shared_ptr<IOCPContext>                m_RecvIOCPContext;
    std::queue<std::shared_ptr<IOCPContext>>    m_SendIOCPContextQueue;

    IOCPSocketContext();
    ~IOCPSocketContext();

    std::shared_ptr<IOCPContext> GetNewIOContext(unsigned __int64 ID);
    bool DeleteContext(const IOCPContext &RemoveIOContext);
};



struct WorkerThreadParam
{
    void*                                       m_Network;
    unsigned long                               m_ThreadID;
    unsigned long                               m_ThreadIndex;
};
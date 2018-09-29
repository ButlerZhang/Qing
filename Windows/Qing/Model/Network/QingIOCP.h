#pragma once
#include "IOCPModel.h"



class QingIOCP
{
public:

    QingIOCP();
    ~QingIOCP();

    bool Start();
    void Stop();

    std::string GetLocalIP();
    void SetListenPort(int ListenPort) { m_ServerPort = ListenPort; }
    void SetServerIP(const std::string &IP) { m_ServerIP = IP; }

protected:

    bool InitializeIOCP();
    bool InitializeListenSocket();
    void DeleteInitializeResources();

    bool PostAccept(IOCPContext *pIOCPContext);
    bool PostRecv(IOCPContext *pIOCPContext);
    bool PostSend(IOCPContext *pIOCPContext);

    bool DoAccept(IOCPSocketContext *pSocketContext, IOCPContext *pIOCPContext);
    bool DoRecv(IOCPSocketContext *pSocketContext, IOCPContext *pIOCPContext);
    bool DoSend(IOCPSocketContext *pSocketContext, IOCPContext *pIOCPContext);

    void AddToContextList(IOCPSocketContext *pSocketContext);
    void RemoveContextList(IOCPSocketContext *pSocketContext);
    void ClearContextList();

    int  GetProcessorsCount();
    bool IsSocketAlive(SOCKET socket);
    void ReleaseHandle(HANDLE Handle);
    bool AssociateWithIOCP(IOCPSocketContext *pSocketContext);
    bool HandleError(IOCPSocketContext *pSocketContext, const DWORD& dwErr);

    static std::string ClientIP(SOCKADDR_IN *ClientAddr);
    static DWORD WINAPI WorkerThread(LPVOID lpParam);

private:

    HANDLE                              m_hShutdownEvent;               //֪ͨ�����߳��˳����¼�
    HANDLE                              m_hIOCompletionPort;            //��ɶ˿ڵľ��
    HANDLE*                             m_phWorkerThreads;              //�����߳�
    int                                 m_nThreads;                     //���ɵ��߳�����
    std::string                         m_ServerIP;                     //����˵�IP
    int                                 m_ServerPort;                   //����˵Ķ˿�
    CRITICAL_SECTION                    m_csContextList;                //Worker�̵߳��ź���
    std::vector<IOCPSocketContext*>     m_ClientContextVector;          //�ͻ���Socket��Context��Ϣ
    IOCPSocketContext*                  m_pListenContext;               //����Socket��Context��Ϣ
    LPFN_ACCEPTEX                       m_lpfnAcceptEx;                 // AcceptEx�ĺ���ָ��
    LPFN_GETACCEPTEXSOCKADDRS           m_lpfnGetAcceptExSockAddrs;     //GetAcceptExSockaddrs�ĺ���ָ��
};

#pragma once
#include "..\..\HeaderFiles\QingBase.h"
#include "ClientManager.h"

QING_NAMESPACE_BEGIN



class QingServer
{
public:

    QingServer();
    ~QingServer();

    bool Start(int ListenPort, const std::string &ServerIP = std::string());
    void Stop();

    const std::string& GetLocalIP();
    int GetServerPort() const { return m_ListenPort; }

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

    int                                 m_WorkerThreadCount;            //�����߳�����
    int                                 m_ListenPort;                   //�����˿�
    std::string                         m_ServerBindIP;                 //����˵�IP
    ClientManager                       m_ClientManager;                //�ͻ��˹�����
    std::shared_ptr<IOCPSocketContext>  m_ListenSocketContext;          //����Socket��Context��Ϣ

    LPFN_ACCEPTEX                       m_CallBackAcceptEx;             //AcceptEx�ĺ���ָ��
    LPFN_GETACCEPTEXSOCKADDRS           m_CallBackGetAcceptExSockAddrs; //GetAcceptExSockaddrs�ĺ���ָ��
};

QING_NAMESPACE_END
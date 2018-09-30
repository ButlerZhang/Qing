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

    HANDLE                              m_hShutdownEvent;               //通知工作线程退出的事件
    HANDLE                              m_hIOCompletionPort;            //完成端口的句柄
    HANDLE*                             m_phWorkerThreads;              //工作线程

    int                                 m_WorkerThreadCount;            //工作线程数量
    int                                 m_ListenPort;                   //侦听端口
    std::string                         m_ServerBindIP;                 //服务端的IP
    ClientManager                       m_ClientManager;                //客户端管理者
    std::shared_ptr<IOCPSocketContext>  m_ListenSocketContext;          //监听Socket的Context信息

    LPFN_ACCEPTEX                       m_CallBackAcceptEx;             //AcceptEx的函数指针
    LPFN_GETACCEPTEXSOCKADDRS           m_CallBackGetAcceptExSockAddrs; //GetAcceptExSockaddrs的函数指针
};

QING_NAMESPACE_END
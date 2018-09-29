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

    HANDLE                              m_hShutdownEvent;               //通知工作线程退出的事件
    HANDLE                              m_hIOCompletionPort;            //完成端口的句柄
    HANDLE*                             m_phWorkerThreads;              //工作线程
    int                                 m_nThreads;                     //生成的线程数量
    std::string                         m_ServerIP;                     //服务端的IP
    int                                 m_ServerPort;                   //服务端的端口
    CRITICAL_SECTION                    m_csContextList;                //Worker线程的信号量
    std::vector<IOCPSocketContext*>     m_ClientContextVector;          //客户端Socket的Context信息
    IOCPSocketContext*                  m_pListenContext;               //监听Socket的Context信息
    LPFN_ACCEPTEX                       m_lpfnAcceptEx;                 // AcceptEx的函数指针
    LPFN_GETACCEPTEXSOCKADDRS           m_lpfnGetAcceptExSockAddrs;     //GetAcceptExSockaddrs的函数指针
};

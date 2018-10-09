#pragma once
#include "NetworkBase.h"
#include "ClientManager.h"

QING_NAMESPACE_BEGIN



class QingServer : public NetworkBase
{
public:

    QingServer();
    virtual ~QingServer();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual void Stop();

protected:

    bool PostAccept(IOCPContext *pIOCPContext);
    bool PostRecv(IOCPContext *pIOCPContext);
    bool PostSend(IOCPContext *pIOCPContext);

    bool ProcessAccept(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext);
    bool ProcessRecv(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext);
    bool ProcessSend(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext);

private:

    bool CreateIOCP();
    bool CreateWorkerThread();
    bool CreateAndStartListen();
    bool InitializeAcceptExCallBack();
    bool StartPostAcceptExIORequest();

    bool IsSocketAlive(SOCKET socket);
    bool HandleError(const std::shared_ptr<IOCPSocketContext> &pSocketContext, DWORD ErrorCode);

    static DWORD WINAPI CallBack_WorkerThread(LPVOID lpParam);

private:

    HANDLE                                  m_hWorkerThreadExitEvent;                   //通知工作线程退出的事件
    HANDLE                                  m_hIOCompletionPort;                        //完成端口的句柄
    HANDLE                                  m_WorkerThreads[MAX_WORKER_THREAD_COUNT];   //工作线程

    ClientManager                           m_ClientManager;                            //客户端管理者
    std::vector<ServerWorkerThreadParam>    m_ThreadParamVector;                        //工作线程参数
    std::shared_ptr<IOCPSocketContext>      m_ListenSocketContext;                      //监听Socket的Context信息

    LPFN_ACCEPTEX                           m_CallBackAcceptEx;                         //AcceptEx的函数指针
    LPFN_GETACCEPTEXSOCKADDRS               m_CallBackGetAcceptExSockAddrs;             //GetAcceptExSockaddrs的函数指针
};

QING_NAMESPACE_END
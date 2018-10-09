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

    HANDLE                                  m_hWorkerThreadExitEvent;                   //֪ͨ�����߳��˳����¼�
    HANDLE                                  m_hIOCompletionPort;                        //��ɶ˿ڵľ��
    HANDLE                                  m_WorkerThreads[MAX_WORKER_THREAD_COUNT];   //�����߳�

    ClientManager                           m_ClientManager;                            //�ͻ��˹�����
    std::vector<ServerWorkerThreadParam>    m_ThreadParamVector;                        //�����̲߳���
    std::shared_ptr<IOCPSocketContext>      m_ListenSocketContext;                      //����Socket��Context��Ϣ

    LPFN_ACCEPTEX                           m_CallBackAcceptEx;                         //AcceptEx�ĺ���ָ��
    LPFN_GETACCEPTEXSOCKADDRS               m_CallBackGetAcceptExSockAddrs;             //GetAcceptExSockaddrs�ĺ���ָ��
};

QING_NAMESPACE_END
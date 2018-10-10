#pragma once
#include "NetworkBase.h"
#include "ClientManager.h"

QING_NAMESPACE_BEGIN



class NetworkServer : public NetworkBase
{
public:

    NetworkServer();
    virtual ~NetworkServer();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual void Stop();

    virtual int Send(int ClientID, const void *MessageData, int MessageSize, __int64 Timeout = 0);

    int GetClientCount() const { return m_ClientManager.GetClientCount(); }

protected:

    bool PostAccept(IOCPContext *pIOCPContext);
    bool PostRecv(IOCPContext *pIOCPContext);
    bool PostSend(IOCPContext *pIOCPContext);

    bool ProcessAccept(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext);
    bool ProcessRecv(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext);
    bool ProcessSend(const std::shared_ptr<IOCPSocketContext> &pSocketContext, IOCPContext *pIOCPContext);

private:

    virtual void WorkerThread();

    bool CreateAndStartListen();
    bool InitializeAcceptExCallBack();
    bool StartPostAcceptExIORequest();

    bool IsSocketAlive(SOCKET socket);
    bool HandleError(const std::shared_ptr<IOCPSocketContext> &pSocketContext, DWORD ErrorCode);

private:

    ClientManager                           m_ClientManager;                            //客户端管理者
    std::shared_ptr<IOCPSocketContext>      m_ListenSocketContext;                      //监听Socket的Context信息
    LPFN_ACCEPTEX                           m_CallBackAcceptEx;                         //AcceptEx的函数指针
    LPFN_GETACCEPTEXSOCKADDRS               m_CallBackGetAcceptExSockAddrs;             //GetAcceptExSockaddrs的函数指针
};

QING_NAMESPACE_END
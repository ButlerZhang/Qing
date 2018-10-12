#pragma once
#include "NetworkBase.h"

QING_NAMESPACE_BEGIN



class NetworkClient : public NetworkBase
{
public:

    NetworkClient();
    virtual ~NetworkClient();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual void Stop();

    virtual int Send(const void *MessageData, int MessageSize);

protected:

    virtual void WorkerThread();

protected:

    bool CreateSocket();
    bool ConnectServer(const std::string &ServerIP, int Port);
    void ReadyToRecvData();

    bool ProcessRecv(IOCPContext &RecvIOCPContext);
    bool ProcessSend(IOCPContext &SendIOCPContext);

private:

    bool                                        m_IsConnected;
    std::shared_ptr<IOCPSocketContext>          m_SocketContext;
};

QING_NAMESPACE_END
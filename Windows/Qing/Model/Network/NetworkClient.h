#pragma once
#include "NetworkBase.h"

QING_NAMESPACE_BEGIN



class NetworkClient : public NetworkBase
{
public:

    NetworkClient();
    virtual ~NetworkClient();

    virtual void Stop();
    virtual bool Start(const std::string &ServerIP, int Port);
    virtual int  Send(const void *MessageData, int MessageSize);

    unsigned __int64 GetClientID() const;
    bool IsConnected() const { return m_IsConnected; }
    bool ConnectServer(const std::string &ServerIP, int Port);

protected:

    bool CreateSocket();
    void WorkerThread();
    void ReadyToRecvData();

    bool ProcessRecv(IOCPContext &RecvIOCPContext);
    bool ProcessSend(IOCPContext &SendIOCPContext);

private:

    bool                                        m_IsConnected;
    std::shared_ptr<IOCPSocketContext>          m_SocketContext;
};

QING_NAMESPACE_END
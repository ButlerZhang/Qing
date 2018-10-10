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

    bool PostRecv(IOCPContext *pIOCPContext);
    bool PostSend(IOCPContext *pIOCPContext);

    bool ProcessRecv(IOCPContext *pIOCPContext);
    bool ProcessSend(IOCPContext *pIOCPContext);

private:

    bool                                        m_IsConnected;                              //�Ƿ�����
    SOCKET                                      m_ClientSocket;                             //socket
    std::shared_ptr<IOCPContext>                m_IOCPContext;
    //std::vector<std::shared_ptr<IOCPContext>>   m_IOContextVector;                          //���ͻ���ܵ���Ϣ
};

QING_NAMESPACE_END
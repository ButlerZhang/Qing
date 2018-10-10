#pragma once
#include "IOCPModel.h"

QING_NAMESPACE_BEGIN



class NetworkBase
{
public:

    NetworkBase();
    virtual ~NetworkBase();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual void Stop();

    virtual int Send(const void *MessageData, int MessageSize) { return 0; }
    virtual int Send(int ClientID, const void *MessageData, int MessageSize, __int64 Timeout = 0) { return 0; }

    const std::string& GetLocalIP();
    int GetServerPort() const { return m_ServerPort; }

protected:

    virtual bool CreateIOCP();
    virtual bool CreateWorkerThread(int WorkerThreadCount = 0);
    virtual void WorkerThread() = 0;

    virtual bool PostRecv(IOCPContext &RecvIOCPContext);
    virtual bool PostSend(IOCPContext &SendIOCPContext);

protected:

    void ReleaseSocket(SOCKET Socket);
    void ReleaseHandle(HANDLE &Handle);

    void SetSocketLinger(SOCKET Socket);
    void FillAddress(sockaddr_in &ServerAddress);

    static DWORD WINAPI CallBack_WorkerThread(LPVOID lpParam);

protected:

    int                                     m_ServerPort;
    std::string                             m_ServerIP;
    HANDLE                                  m_hIOCompletionPort;
    HANDLE                                  m_hWorkerThreadExitEvent;
    HANDLE                                  m_WorkerThreads[MAX_WORKER_THREAD_COUNT];
    std::vector<WorkerThreadParam>          m_ThreadParamVector;
};

QING_NAMESPACE_END
#pragma once
#include "IOCPModel.h"
#include <string>



class NetworkBase
{
public:

    NetworkBase();
    virtual ~NetworkBase();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual void Stop();

    virtual bool IsRunning() const;
    virtual int  Send(const void *MessageData, int MessageSize) { return 0; }
    virtual int  Send(SOCKET ClientID, const void *MessageData, int MessageSize, __int64 Timeout = 0) { return 0; }

    const std::string& GetLocalIP();
    int GetServerPort() const { return m_ServerPort; }

protected:

    virtual bool CreateWorkerThread(int WorkerThreadCount = 0);
    virtual void WorkerThread() = 0;

    virtual bool PostRecv(IOCPContext &RecvIOCPContext);
    virtual bool PostSend(IOCPContext &SendIOCPContext);

protected:

    void ReleaseHandle(HANDLE &Handle);
    void SetSocketLinger(SOCKET Socket);
    void FillAddress(sockaddr_in &ServerAddress);
    static DWORD WINAPI CallBack_WorkerThread(LPVOID lpParam);

protected:

    int                                     m_ServerPort;
    std::string                             m_ServerIP;
    HANDLE                                  m_hWorkerThreadExitEvent;
    HANDLE                                  m_WorkerThreads[MAX_WORKER_THREAD_COUNT];
    std::vector<WorkerThreadParam>          m_ThreadParamVector;
};

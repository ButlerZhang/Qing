#pragma once
#include "IOCPModel.h"
#include <string>

QING_NAMESPACE_BEGIN



class NetworkBase
{
public:

    NetworkBase();
    virtual ~NetworkBase();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual void Stop();

    virtual int Send(const void *MessageData, int MessageSize) { return 0; }
    virtual int Send(int NaturalIndex, const void *MessageData, int MessageSize, __int64 Timeout = 0) { return 0; }

    const std::string& GetLocalIP();
    int GetServerPort() const { return m_ServerListenPort; }

protected:

    virtual bool CreateIOCP();
    virtual bool CreateWorkerThread(int WorkerThreadCount = 0);
    virtual void WorkerThread() = 0;

protected:

    void ReleaseSocket(SOCKET Socket);
    void ReleaseHandle(HANDLE &Handle);

    void SetSocketLinger(SOCKET Socket);
    void FillAddress(sockaddr_in &ServerAddress);

    static DWORD WINAPI CallBack_WorkerThread(LPVOID lpParam);

protected:

    int                                     m_ServerListenPort;                         //侦听端口
    std::string                             m_ServerIP;                                 //绑定IP
    HANDLE                                  m_hIOCompletionPort;                        //完成端口的句柄
    HANDLE                                  m_hWorkerThreadExitEvent;                   //通知工作线程退出的事件
    HANDLE                                  m_WorkerThreads[MAX_WORKER_THREAD_COUNT];   //工作线程
    std::vector<ServerWorkerThreadParam>    m_ThreadParamVector;                        //工作线程参数
};

QING_NAMESPACE_END
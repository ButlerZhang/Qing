#pragma once
#include "NetworkBase.h"
#include "IOCPModel.h"

QING_NAMESPACE_BEGIN



class QingClient : public NetworkBase
{
public:

    QingClient();
    virtual ~QingClient();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual void Stop();

protected:

    bool ConnectServer(SOCKET *pSocket, std::string ServerIP, int nPort);

    static DWORD WINAPI CallBack_ConnectThread(LPVOID lpParam);
    static DWORD WINAPI CallBack_WorkerThread(LPVOID lpParam);

private:

    HANDLE                                  m_hConnectThread;                           //接受连接的线程句柄
    HANDLE                                  m_hShutdownEvent;                           //线程退出事件
    HANDLE                                 *m_phWorkerThreads;                          //工作线程
    std::vector<ClientWorkerThreadParam>    m_ThreadParamVector;                        //工作线程参数
};

QING_NAMESPACE_END
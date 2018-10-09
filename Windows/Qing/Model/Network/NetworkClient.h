#pragma once
#include "NetworkBase.h"
#include "IOCPModel.h"

QING_NAMESPACE_BEGIN



class NetworkClient : public NetworkBase
{
public:

    NetworkClient();
    virtual ~NetworkClient();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual void Stop();

protected:

    bool CreateSocket();
    bool ConnectServer(const std::string &ServerIP, int Port);

    bool PostRecv(std::shared_ptr<IOCPContext> &pIOCPContext);
    bool PostSend(std::shared_ptr<IOCPContext> &pIOCPContext);

    bool ConnectServer(SOCKET *pSocket, std::string ServerIP, int nPort);
    static DWORD WINAPI CallBack_ConnectThread(LPVOID lpParam);
    static DWORD WINAPI CallBack_WorkerThread(LPVOID lpParam);

private:

    bool                                        m_IsConnected;
    SOCKET                                      m_ClientSocket;
    std::vector<std::shared_ptr<IOCPContext>>   m_IOContextVector;


    HANDLE                                  m_hConnectThread;                           //接受连接的线程句柄
    HANDLE                                  m_hShutdownEvent;                           //线程退出事件
    HANDLE                                 *m_phWorkerThreads;                          //工作线程
    std::vector<ClientWorkerThreadParam>    m_ThreadParamVector;                        //工作线程参数
};

QING_NAMESPACE_END
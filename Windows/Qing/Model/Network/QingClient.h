#pragma once
#include "..\..\HeaderFiles\LocalComputer.h"
#include "IOCPModel.h"

QING_NAMESPACE_BEGIN



class QingClient
{
public:

    QingClient();
    ~QingClient();

    bool Start(const std::string &ServerIP, int ServerPort);
    void Stop();

    const std::string& GetLocalIP();
    int GetServerPort() const { return m_ListenPort; }

protected:

    void CleanUp();
    void ReleaseHandle(HANDLE &Handle);

    bool EstablishConnections(int ThreadCount);
    bool ConnectServer(SOCKET *pSocket, std::string ServerIP, int nPort);

    static DWORD WINAPI CallBack_ConnectThread(LPVOID lpParam);
    static DWORD WINAPI CallBack_WorkerThread(LPVOID lpParam);

private:

    int                                     m_ListenPort;                               //侦听端口
    std::string                             m_ServerIP;                                 //服务端的IP
    HANDLE                                  m_hConnectThread;                           //接受连接的线程句柄
    HANDLE                                  m_hShutdownEvent;                           //线程退出事件
    HANDLE                                 *m_phWorkerThreads;                          //工作线程
    LocalComputer                           m_LocalComputer;                            //本地机器信息
    std::vector<ClientWorkerThreadParam>    m_ThreadParamVector;                        //工作线程参数
};

QING_NAMESPACE_END
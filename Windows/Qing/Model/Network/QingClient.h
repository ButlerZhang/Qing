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

    HANDLE                                  m_hConnectThread;                           //�������ӵ��߳̾��
    HANDLE                                  m_hShutdownEvent;                           //�߳��˳��¼�
    HANDLE                                 *m_phWorkerThreads;                          //�����߳�
    std::vector<ClientWorkerThreadParam>    m_ThreadParamVector;                        //�����̲߳���
};

QING_NAMESPACE_END
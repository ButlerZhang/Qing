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


    HANDLE                                  m_hConnectThread;                           //�������ӵ��߳̾��
    HANDLE                                  m_hShutdownEvent;                           //�߳��˳��¼�
    HANDLE                                 *m_phWorkerThreads;                          //�����߳�
    std::vector<ClientWorkerThreadParam>    m_ThreadParamVector;                        //�����̲߳���
};

QING_NAMESPACE_END
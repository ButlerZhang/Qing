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

    int                                     m_ListenPort;                               //�����˿�
    std::string                             m_ServerIP;                                 //����˵�IP
    HANDLE                                  m_hConnectThread;                           //�������ӵ��߳̾��
    HANDLE                                  m_hShutdownEvent;                           //�߳��˳��¼�
    HANDLE                                 *m_phWorkerThreads;                          //�����߳�
    LocalComputer                           m_LocalComputer;                            //���ػ�����Ϣ
    std::vector<ClientWorkerThreadParam>    m_ThreadParamVector;                        //�����̲߳���
};

QING_NAMESPACE_END
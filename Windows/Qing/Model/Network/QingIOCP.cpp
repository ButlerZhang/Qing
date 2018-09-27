#include "QingIOCP.h"
#include "..\..\HeaderFiles\QingLog.h"


struct WorkerThreadParam
{
    QingIOCP    *m_pQingIOCP;       //���ڵ������еĺ���
    int          m_nThreadNo;       //�̱߳��
};



QingIOCP::QingIOCP() : m_hShutdownEvent(NULL),
                       m_hIOCompletionPort(NULL),
                       m_phWorkerThreads(NULL),
                       m_nThreads(0),
                       m_ServerIP("127.0.0.1"),
                       m_ServerPort(8888),
                       m_pListenContext(NULL),
                       m_lpfnAcceptEx(NULL),
                       m_lpfnGetAcceptExSockAddrs(NULL)
{
}

QingIOCP::~QingIOCP()
{
    Stop();
}

bool QingIOCP::Start()
{
    //��ʼ���߳��ź���
    InitializeCriticalSection(&m_csContextList);

    //�����߳��˳����¼�֪ͨ
    m_hShutdownEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

    //��ʼ��IOCP
    if (!InitializeIOCP())
    {
        Qing::QingLog::Write("Initialize IOCP failed.", Qing::LL_ERROR);
        return false;
    }

    Qing::QingLog::Write("Initialize IOCP succeed.", Qing::LL_INFO);

    //��ʼ��Socket
    if (!InitializeListenSocket())
    {
        Qing::QingLog::Write("Initialize listen socket failed.", Qing::LL_ERROR);
        DeleteInitializeResources();
        return false;
    }

    Qing::QingLog::Write("Initialize listen socket succeed.", Qing::LL_INFO);
    Qing::QingLog::Write("QingIOCP ready.", Qing::LL_INFO);
    return true;
}

void QingIOCP::Stop()
{
    if (m_pListenContext != NULL && m_pListenContext->m_Socket != INVALID_SOCKET)
    {
        //����ر���Ϣ֪ͨ
        SetEvent(m_hShutdownEvent);

        //֪ͨ���е���ɶ˿ڲ����˳�
        for (int i = 0; i < m_nThreads; i++)
        {
            PostQueuedCompletionStatus(m_hIOCompletionPort, 0, NULL, NULL);
        }

        //�ȴ����еĿͻ�����Դ�˳�
        WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);

        //����ͻ����б���Ϣ
        ClearContextList();

        //�ͷ�������Դ
        DeleteInitializeResources();

        Qing::QingLog::Write("Stop listen.", Qing::LL_INFO);
    }
}

std::string QingIOCP::GetLocalIP()
{
    char HostName[MAX_PATH];
    memset(HostName, 0, MAX_PATH);

    //��ȡ������
    gethostname(HostName, MAX_PATH);
    hostent FAR* lpHostEnt = gethostbyname(HostName);
    if (lpHostEnt == NULL)
    {
        return m_ServerIP;
    }

    //��ȡIP��ַ�б��еĵ�һ��IP
    LPSTR lpAddr = lpHostEnt->h_addr_list[0];

    //��IP��ַת�����ַ�����ʽ
    struct in_addr inAddr;
    memmove(&inAddr, lpAddr, 4);
    m_ServerIP = inet_ntoa(inAddr);

    return m_ServerIP;
}

bool QingIOCP::InitializeIOCP()
{
    //������ɶ˿�
    m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == m_hIOCompletionPort)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Create IO completion port error = %d.", WSAGetLastError());
        return false;
    }

    //���ݴ���������������Ӧ���߳���
    m_nThreads = WORKER_THREADS_PER_PROCESSOR * GetProcessorsCount();
    m_phWorkerThreads = new HANDLE[m_nThreads];

    //����worker�߳�
    DWORD nThreadID;
    for (int i = 0; i < m_nThreads; i++)
    {
        WorkerThreadParam *pWorkerThreadParam = new WorkerThreadParam();
        pWorkerThreadParam->m_pQingIOCP = this;
        pWorkerThreadParam->m_nThreadNo = i + 1;
        m_phWorkerThreads[i] = ::CreateThread(0, 0, WorkerThread, (void*)pWorkerThreadParam, 0, &nThreadID);
    }

    Qing::QingLog::Write(Qing::LL_INFO, "Created %d worker threads.", m_nThreads);
    return true;
}

#include "QingIOCP.h"
#include "..\..\HeaderFiles\QingLog.h"


struct WorkerThreadParam
{
    QingIOCP    *m_pQingIOCP;       //用于调用类中的函数
    int          m_nThreadNo;       //线程编号
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
    //初始化线程信号量
    InitializeCriticalSection(&m_csContextList);

    //建立线程退出的事件通知
    m_hShutdownEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

    //初始化IOCP
    if (!InitializeIOCP())
    {
        Qing::QingLog::Write("Initialize IOCP failed.", Qing::LL_ERROR);
        return false;
    }

    Qing::QingLog::Write("Initialize IOCP succeed.", Qing::LL_INFO);

    //初始化Socket
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
        //激活关闭消息通知
        SetEvent(m_hShutdownEvent);

        //通知所有的完成端口操作退出
        for (int i = 0; i < m_nThreads; i++)
        {
            PostQueuedCompletionStatus(m_hIOCompletionPort, 0, NULL, NULL);
        }

        //等待所有的客户端资源退出
        WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);

        //清除客户端列表信息
        ClearContextList();

        //释放其它资源
        DeleteInitializeResources();

        Qing::QingLog::Write("Stop listen.", Qing::LL_INFO);
    }
}

std::string QingIOCP::GetLocalIP()
{
    char HostName[MAX_PATH];
    memset(HostName, 0, MAX_PATH);

    //获取主机名
    gethostname(HostName, MAX_PATH);
    hostent FAR* lpHostEnt = gethostbyname(HostName);
    if (lpHostEnt == NULL)
    {
        return m_ServerIP;
    }

    //获取IP地址列表中的第一个IP
    LPSTR lpAddr = lpHostEnt->h_addr_list[0];

    //将IP地址转换成字符串形式
    struct in_addr inAddr;
    memmove(&inAddr, lpAddr, 4);
    m_ServerIP = inet_ntoa(inAddr);

    return m_ServerIP;
}

bool QingIOCP::InitializeIOCP()
{
    //创建完成端口
    m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == m_hIOCompletionPort)
    {
        Qing::QingLog::Write(Qing::LL_ERROR, "Create IO completion port error = %d.", WSAGetLastError());
        return false;
    }

    //根据处理器个数创建对应的线程数
    m_nThreads = WORKER_THREADS_PER_PROCESSOR * GetProcessorsCount();
    m_phWorkerThreads = new HANDLE[m_nThreads];

    //创建worker线程
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

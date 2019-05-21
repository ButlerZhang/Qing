#include "NetworkEnvironment.h"
#include "../../Boost/BoostLog.h"
#include "../../Boost/BoostFormat.h"

QING_NAMESPACE_BEGIN



NetworkEnvironment::NetworkEnvironment() : g_IOCPContextTrackID(0),
                                           g_NetworkInitCounter(0),
                                           g_hIOCompletionPort(NULL)

{
    StartupNetwork();
}

NetworkEnvironment::~NetworkEnvironment()
{
    ShutdownNetwork();
}

NetworkEnvironment& NetworkEnvironment::GetInstance()
{
    static NetworkEnvironment Instance;
    return Instance;
}

unsigned __int64 NetworkEnvironment::GetNextTrackID()
{
    InterlockedIncrement(&g_IOCPContextTrackID);
    return g_IOCPContextTrackID;
}

HANDLE NetworkEnvironment::GetIOCP()
{
    if (g_hIOCompletionPort == NULL)
    {
        g_hIOCompletionPort = CreateIoCompletionPort(
            INVALID_HANDLE_VALUE,   //FileHandle是有效的文件句柄或INVALID_HANDLE_VALUE
            NULL,                   //ExistingCompletionPort已经存在的完成端口，如果为NULL则新建一个
            0,                      //CompletionKey是传送给处理函数的参数
            0);                     //NumberOfConcurrentThreads有多少个线程在访问这个消息队列
                                    //如果设置为0表示有多少个处理器就允许同时运行多少个线程
                                    //如果ExistingCompletionPort不为0，系统忽略NumberOfConcurrentThreads

        if (g_hIOCompletionPort == NULL)
        {
            BoostLog::WriteError(BoostFormat(L"Create IO completion port error = %d.", WSAGetLastError()));
        }
        else
        {
            BoostLog::WriteInfo(L"Created IO completion port succeed.");
        }
    }

    return g_hIOCompletionPort;
}

bool NetworkEnvironment::StartupNetwork()
{
    //原子加，保证只调用一次WSAStartup
    if (InterlockedIncrement(&g_NetworkInitCounter) == 1)
    {
        WSAData wsaData;
        //WSAStartup用来初始化Windows Sockets DLL，成功后才能调用其它API
        //MAKEWORD(2,2)表示使用的是WinSocket2.2版本
        int Result = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (Result == NO_ERROR)
        {
            return true;
        }

        BoostLog::WriteError(BoostFormat(L"WSAStartup error = %d.", Result));
    }

    return false;
}

bool NetworkEnvironment::ShutdownNetwork()
{
    //原子减，保证只调用一次WSACleanup
    if (InterlockedDecrement(&g_NetworkInitCounter) == 0)
    {
        //终止Winsock 2 DLL的使用
        int Result = ::WSACleanup();
        if (Result == NO_ERROR)
        {
            return true;
        }

        BoostLog::WriteError(BoostFormat(L"WSACleanup error = %d.", Result));
    }

    return false;
}

QING_NAMESPACE_END
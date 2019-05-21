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
            INVALID_HANDLE_VALUE,   //FileHandle����Ч���ļ������INVALID_HANDLE_VALUE
            NULL,                   //ExistingCompletionPort�Ѿ����ڵ���ɶ˿ڣ����ΪNULL���½�һ��
            0,                      //CompletionKey�Ǵ��͸��������Ĳ���
            0);                     //NumberOfConcurrentThreads�ж��ٸ��߳��ڷ��������Ϣ����
                                    //�������Ϊ0��ʾ�ж��ٸ�������������ͬʱ���ж��ٸ��߳�
                                    //���ExistingCompletionPort��Ϊ0��ϵͳ����NumberOfConcurrentThreads

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
    //ԭ�Ӽӣ���ֻ֤����һ��WSAStartup
    if (InterlockedIncrement(&g_NetworkInitCounter) == 1)
    {
        WSAData wsaData;
        //WSAStartup������ʼ��Windows Sockets DLL���ɹ�����ܵ�������API
        //MAKEWORD(2,2)��ʾʹ�õ���WinSocket2.2�汾
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
    //ԭ�Ӽ�����ֻ֤����һ��WSACleanup
    if (InterlockedDecrement(&g_NetworkInitCounter) == 0)
    {
        //��ֹWinsock 2 DLL��ʹ��
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
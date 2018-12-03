#pragma once
#include "IOCPModel.h"

QING_NAMESPACE_BEGIN



class NetworkEnvironment
{
public:

    NetworkEnvironment();
    ~NetworkEnvironment();

    HANDLE GetIOCP();
    unsigned __int64  GetNextTrackID();
    static NetworkEnvironment& GetInstance();

private:

    bool StartupNetwork();
    bool ShutdownNetwork();

private:

    HANDLE                           g_hIOCompletionPort;
    unsigned __int64                 g_IOCPContextTrackID;
    long                             g_NetworkInitCounter;
};

#define GlobalNetwork NetworkEnvironment::GetInstance()

QING_NAMESPACE_END
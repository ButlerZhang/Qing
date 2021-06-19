#pragma once
#include "LibeventInterface.h"



class UDPBroadcast
{
public:

    UDPBroadcast();
    ~UDPBroadcast();

    bool StartTimer(event_base *EventBase, const std::string &ServerIP, int Port, int TimeInternal = 60);

    inline bool GetIsDisplayLog() const { return m_IsDisplayLog; }
    inline void SetIsDisplayLog(bool IsDisplayLog) { m_IsDisplayLog = IsDisplayLog; }

private:

    static void CallBack_TimeOut(int Socket, short Events, void *UserData);

private:

    bool                                 m_IsDisplayLog;
    int                                  m_TimeInternal;
    int                                  m_BroadcastPort;
    int                                  m_BroadcastSocket;
    std::string                          m_BroadcastServerIP;
    EventNormal                          m_TimeoutEvent;
    struct timeval                       m_LastSendTime;
    struct sockaddr_in                   m_BroadcastAddress;
};

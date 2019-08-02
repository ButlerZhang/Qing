#pragma once
#include <string>
#include <event.h>



class UDPBroadcast
{
public:

    UDPBroadcast();
    ~UDPBroadcast();

    bool BindBaseEvent(event_base *EventBase);
    bool StartTimer(const std::string &ServerIP, int TimeInternal = 60, int Port = 12345);

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
    struct timeval                       m_LastSendTime;
    struct event                        *m_TimeoutEvent;
    struct event_base                   *m_EventBase;
    struct sockaddr_in                   m_BroadcastAddress;
};
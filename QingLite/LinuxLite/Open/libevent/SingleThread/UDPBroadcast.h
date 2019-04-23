#pragma once
#include <event.h>



class UDPBroadcast
{
public:

    UDPBroadcast();
    ~UDPBroadcast();

    bool BindEventBase(event_base *EventBase);
    bool StartTimer(int TimeInternal = 60, int Port = 12345);

private:

    static void CallBack_TimeOut(evutil_socket_t Socket, short Events, void *UserData);

private:

    int                             m_TimeInternal;
    timeval                         m_LastSendTime;
    evutil_socket_t                 m_UDPBroadcastSocket;

    struct event                    m_TimeoutEvent;
    struct event_base              *m_EventBase;
    struct sockaddr_in              m_BroadcastAddress;
};

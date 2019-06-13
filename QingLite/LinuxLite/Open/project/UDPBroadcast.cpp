#include "UDPBroadcast.h"
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <string>



UDPBroadcast::UDPBroadcast()
{
    m_EventBase = NULL;
    m_TimeoutEvent = NULL;
    m_IsDisplayLog = false;

    m_TimeInternal = 60;
    m_BroadcastPort = 0;
    m_BroadcastSocket = -1;
}

UDPBroadcast::~UDPBroadcast()
{
    m_EventBase = NULL;
    event_free(m_TimeoutEvent);
    close(m_BroadcastSocket);
}

bool UDPBroadcast::BindBaseEvent(event_base *EventBase)
{
    m_EventBase = EventBase;
    return m_EventBase != NULL;
}

bool UDPBroadcast::StartTimer(const std::string &ServerIP, int TimeInternal, int Port)
{
    if (TimeInternal <= 0)
    {
        printf("ERROR: Timer at least needs one second.\n");
        return false;
    }

    if (m_EventBase == NULL)
    {
        printf("ERROR: No binding event base.\n");
        return false;
    }

    m_BroadcastSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_BroadcastSocket == -1)
    {
        printf("ERROR: Create udp socket error.\n");
        return false;
    }

    int Optval = 1;
    if (setsockopt(m_BroadcastSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &Optval, sizeof(int)) < 0)
    {
        printf("ERROR: setsockopt error.\n");
        return false;
    }

    m_BroadcastPort = Port;
    m_BroadcastServerIP = ServerIP;

    bzero(&m_BroadcastAddress, sizeof(sockaddr_in));
    m_BroadcastAddress.sin_family = AF_INET;
    inet_pton(AF_INET, "255.255.255.255", &m_BroadcastAddress.sin_addr);
    m_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(m_BroadcastPort));

    m_TimeInternal = TimeInternal;
    m_TimeoutEvent = event_new(m_EventBase, -1, EV_PERSIST, CallBack_TimeOut, this);

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = m_TimeInternal;
    event_add(m_TimeoutEvent, &tv);

    evutil_gettimeofday(&m_LastSendTime, NULL);
    return true;
}

void UDPBroadcast::CallBack_TimeOut(int Socket, short Events, void *UserData)
{
    UDPBroadcast *Broadcast = (UDPBroadcast*)UserData;

    const std::string UDPData = Broadcast->m_BroadcastServerIP + ":" + std::to_string(Broadcast->m_BroadcastPort);
    ssize_t SendSize = sendto(
        Broadcast->m_BroadcastSocket,
        UDPData.c_str(),
        UDPData.length(),
        0,
        (struct sockaddr*)&(Broadcast->m_BroadcastAddress),
        sizeof(Broadcast->m_BroadcastAddress));
    assert(SendSize == static_cast<ssize_t>(UDPData.length()));

    struct timeval NewTime, DifferentTime;
    evutil_gettimeofday(&NewTime, NULL);
    evutil_timersub(&NewTime, &(Broadcast->m_LastSendTime), &DifferentTime);
    double elapsed = static_cast<double>(DifferentTime.tv_sec) +
        (static_cast<double>(DifferentTime.tv_usec) / 1.0e6);

    if (Broadcast->m_IsDisplayLog)
    {
        printf("Broadcast = %s, elapsed %.3f seconds.\n", UDPData.c_str(), elapsed);
    }

    Broadcast->m_LastSendTime = NewTime;

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = Broadcast->m_TimeInternal;
    event_add(Broadcast->m_TimeoutEvent, &tv);
}

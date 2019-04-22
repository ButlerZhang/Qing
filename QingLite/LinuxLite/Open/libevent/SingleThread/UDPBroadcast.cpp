#include "UDPBroadcast.h"
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <string>



UDPBroadcast::UDPBroadcast()
{
    m_EventBase = NULL;
}

UDPBroadcast::~UDPBroadcast()
{
    m_EventBase = NULL;
}

bool UDPBroadcast::BindEventBase(event_base *EventBase)
{
    m_EventBase = EventBase;
    return m_EventBase != NULL;
}

bool UDPBroadcast::StartTimer(int Seconds, int Port)
{
    m_UDPBroadcastSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_UDPBroadcastSocket == -1)
    {
        printf("ERROR: Create udp socket error.\n\n");
        return false;
    }

    int iOptval = 1;
    if (setsockopt(m_UDPBroadcastSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &iOptval, sizeof(int)) < 0)
    {
        printf("ERROR: setsockopt failed!");
        return false;
    }

    bzero(&m_BroadcastAddress, sizeof(sockaddr_in));
    m_BroadcastAddress.sin_family = AF_INET;
    inet_pton(AF_INET, "255.255.255.255", &m_BroadcastAddress.sin_addr);
    m_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(Port));

    m_TimeInternal = Seconds;
    event_assign(&m_TimeoutEvent, m_EventBase, -1, EV_PERSIST, CallBack_UDPBroadcastTimeOut, (void*)this);

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = m_TimeInternal;
    event_add(&(m_TimeoutEvent), &tv);

    evutil_gettimeofday(&m_LastSendTime, NULL);
    return true;
}

void UDPBroadcast::CallBack_UDPBroadcastTimeOut(evutil_socket_t Socket, short events, void * UserData)
{
    UDPBroadcast *Broadcast = (UDPBroadcast*)UserData;

    std::string Message("192.168.3.126");
    ssize_t SendSize = sendto(Broadcast->m_UDPBroadcastSocket, Message.c_str(), Message.length(), 0,
        (struct sockaddr*)&(Broadcast->m_BroadcastAddress), sizeof(Broadcast->m_BroadcastAddress));
    assert(SendSize == static_cast<ssize_t>(Message.length()));

    struct timeval NewTime, DifferentTime;
    evutil_gettimeofday(&NewTime, NULL);
    evutil_timersub(&NewTime, &(Broadcast->m_LastSendTime), &DifferentTime);
    double elapsed = static_cast<double>(DifferentTime.tv_sec) +
        (static_cast<double>(DifferentTime.tv_usec) / 1.0e6);

    printf("Broadcast = %s, %d: %.3f seconds elapsed.\n\n",
        Message.c_str(), (int)NewTime.tv_sec, elapsed);

    Broadcast->m_LastSendTime = NewTime;

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = Broadcast->m_TimeInternal;
    event_add((struct event*)&(Broadcast->m_TimeoutEvent), &tv);
}

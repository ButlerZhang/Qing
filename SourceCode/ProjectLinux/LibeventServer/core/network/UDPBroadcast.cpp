#include "UDPBroadcast.h"
#include "../../../../Share/Boost/BoostLog.h"
#include <arpa/inet.h>
#include <assert.h>



UDPBroadcast::UDPBroadcast()
{
    m_IsDisplayLog = false;
    m_TimeInternal = 60;
    m_BroadcastPort = 0;
    m_BroadcastSocket = -1;
}

UDPBroadcast::~UDPBroadcast()
{
    if (m_BroadcastSocket > 0)
    {
        close(m_BroadcastSocket);
        m_BroadcastSocket = -1;
    }

    g_Log.WriteDebug("UDPBroadcast was destructored.");
}

bool UDPBroadcast::StartTimer(event_base *EventBase, const std::string &ServerIP, int Port, int TimeInternal)
{
    if (EventBase == NULL)
    {
        g_Log.WriteError("UDPBroadcast event base is NULL.");
        return false;
    }

    if (TimeInternal <= 0)
    {
        g_Log.WriteError("UDPBroadcast time internal is less than 0.");
        return false;
    }

    m_BroadcastSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_BroadcastSocket == -1)
    {
        g_Log.WriteError(BoostFormat("UDPBroadcast create udp socket error = %s.", strerror(errno)));
        return false;
    }

    int Optval = 1;
    if (setsockopt(m_BroadcastSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &Optval, sizeof(int)) < 0)
    {
        g_Log.WriteError(BoostFormat("UDPBroadcast setsockopt error = %s.", strerror(errno)));
        return false;
    }

    m_BroadcastPort = Port;
    m_BroadcastServerIP = ServerIP;

    bzero(&m_BroadcastAddress, sizeof(sockaddr_in));
    m_BroadcastAddress.sin_family = AF_INET;
    inet_pton(AF_INET, "255.255.255.255", &m_BroadcastAddress.sin_addr);
    m_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(m_BroadcastPort));

    m_TimeInternal = TimeInternal;
    m_TimeoutEvent.m_event = event_new(EventBase, -1, EV_PERSIST, CallBack_TimeOut, this);

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = m_TimeInternal;
    if (event_add(m_TimeoutEvent.m_event, &tv) != 0)
    {
        g_Log.WriteError("UDPBroadcast add time out event failed.");
        return false;
    }

    evutil_gettimeofday(&m_LastSendTime, NULL);
    return true;
}

void UDPBroadcast::CallBack_TimeOut(int Socket, short Events, void *UserData)
{
    UDPBroadcast *Broadcast = (UDPBroadcast*)UserData;

    const std::string &UDPData = Broadcast->m_BroadcastServerIP + ":" + std::to_string(Broadcast->m_BroadcastPort);
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
        g_Log.WriteDebug(BoostFormat("UDP Broadcast data = %s, size = %d, elapsed %.3f seconds.", UDPData.c_str(), SendSize, elapsed));
    }

    Broadcast->m_LastSendTime = NewTime;

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = Broadcast->m_TimeInternal;
    event_add(Broadcast->m_TimeoutEvent.m_event, &tv);
}

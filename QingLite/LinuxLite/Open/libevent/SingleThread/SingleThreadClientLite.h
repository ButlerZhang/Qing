#pragma once
#include <event.h>
#include <string>



class SingleThreadClientLite
{
public:

    SingleThreadClientLite();
    ~SingleThreadClientLite();

    bool Start();
    bool Stop();

private:

    bool RecvUDPBroadcast();
    bool ConnectServer(const std::string &ServerIP, int Port);

    static void CallBack_UDPBroadcastRecv(int Socket, short events, void *UserData);

private:

    std::string                      m_ServerIP;
    int                              m_ServerPort;

    int                              m_UDPSocket;
    struct sockaddr_in               m_BroadcastAddress;

    event_base                      *m_EventBase;
};

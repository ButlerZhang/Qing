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
    bool UnbindUDPBroadcast();
    bool ConnectServer(const std::string &ServerIP, int Port);

private:

    static void CallBack_RecvUDPBroadcast(int Socket, short Events, void *UserData);
    static void CallBack_ClientEvent(struct bufferevent *bev, short Events, void *UserData);
    static void CallBack_InputFromCMD(int Input, short Events, void *UserData);
    static void CallBack_RecvFromServer(struct bufferevent *bev, void *UserData);

private:

    std::string                      m_ServerIP;
    int                              m_ServerPort;
    int                              m_UDPSocket;
    struct sockaddr_in               m_BroadcastAddress;
    struct event_base               *m_EventBase;
    struct event                    *m_UDPBroadcastEvent;
};

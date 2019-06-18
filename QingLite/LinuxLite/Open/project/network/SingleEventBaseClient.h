#pragma once
#include <event.h>
#include <string>
#include <random>



class SingleEventBaseClient
{
public:

    SingleEventBaseClient();
    ~SingleEventBaseClient();

    bool Start(const std::string &ServerIP, int Port);
    bool Start(int UDPBroadcastPort);
    bool Stop();

    inline bool IsConnected() const { return m_IsConnected; }

private:

    bool AddEventInputFromCMD();
    bool AddEventRecvUDPBroadcast();

    bool AddTimerReBindUDPSocket();
    bool AddTimerReConnectServer();
    bool AddTimerSendDataRandomly();

    bool ConnectServer(const std::string &ServerIP, int Port);

private:

    static void CallBack_InputFromCMD(int Input, short Events, void *UserData);
    static void CallBack_RecvUDPBroadcast(int Socket, short Events, void *UserData);
    static void CallBack_ReBindUDPSocket(int Socket, short Events, void *UserData);
    static void CallBack_ReConnectServer(int Socket, short Events, void *UserData);
    static void CallBack_SendDataRandomly(int Socket, short Events, void *UserData);
    static void CallBack_ClientEvent(struct bufferevent *bev, short Events, void *UserData);
    static void CallBack_RecvFromServer(struct bufferevent *bev, void *UserData);

private:

    bool                                         m_IsConnected;
    std::string                                  m_ServerIP;
    int                                          m_ServerPort;
    int                                          m_BroadcastPort;
    int                                          m_UDPSocket;
    struct sockaddr_in                           m_BroadcastAddress;
    struct bufferevent                          *m_DataBufferevent;
    struct event_base                           *m_EventBase;
    struct event                                *m_CMDInputEvent;
    struct event                                *m_UDPBroadcastEvent;
    struct event                                *m_ReBindUDPSocketTimer;
    struct event                                *m_ReConnectServerTimer;
    struct event                                *m_SendDataRandomlyTimer;
};

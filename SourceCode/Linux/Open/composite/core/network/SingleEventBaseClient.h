#pragma once
#include "NetworkBase.h"



class SingleEventBaseClient
{
public:

    SingleEventBaseClient();
    virtual ~SingleEventBaseClient();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual bool Start(int UDPBroadcastPort);

    inline bool IsConnected() const { return m_IsConnected; }

protected:

    bool Send(const void *Data, size_t Size);

    virtual bool ProcessConnected() { return false; }
    virtual bool ProcessDisconnected() { return false; }
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg) { return false; }

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
    std::shared_ptr<EventIOBuffer>               m_IOBuffer;
    EventBase                                    m_EventBase;
    EventDataBuffer                              m_RecvDataBuffer;
    EventNormal                                  m_CMDInputEvent;
    EventNormal                                  m_UDPBroadcastEvent;
    EventNormal                                  m_ReBindUDPSocketTimer;
    EventNormal                                  m_ReConnectServerTimer;
    EventNormal                                  m_SendDataRandomlyTimer;
};

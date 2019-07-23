#pragma once
#include "UDPBroadcast.h"
#include "SignalEventMap.h"
#include "ServerNetworkMessageHandler.h"
#include <vector>
#include <event2/listener.h>



class SingleEventBaseServer
{
public:

    SingleEventBaseServer();
    virtual ~SingleEventBaseServer();

    virtual bool Start(const std::string &IP, int Port);
    virtual bool Stop();

    virtual bool ProcessCheckout() { return false; }
    virtual bool ProcessConnected() { return false; }
    virtual bool ProcessDisconnected() { return false; }
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg);

protected:

    inline event_base* GetEventBase() const { return m_EventBase; }

    bool AddCheckoutTimer(int TimerInternal);
    bool CreateListener(const std::string &IP, int Port);
    bool Send(const NetworkMessage &NetworkMsg, const void *Data, size_t Size);

private:

    static void CallBack_Listen(evconnlistener *Listener, int Socket, sockaddr *Address, int SocketLen, void *UserData);
    static void CallBack_Checkout(int Socket, short Events, void *UserData);
    static void CallBack_Signal(int Signal, short Events, void *UserData);

    static void CallBack_Event(struct bufferevent *bev, short Events, void *UserData);
    static void CallBack_Recv(struct bufferevent *bev, void *UserData);
    static void CallBack_Send(struct bufferevent *bev, void *UserData);

private:

    int                                  m_ListenPort;
    std::string                          m_ListenIP;
    event_base                          *m_EventBase;
    evconnlistener                      *m_Listener;
    event                               *m_CheckoutTimer;
    UDPBroadcast                         m_UDPBroadcast;
    SignalEventMap                       m_SignalEventMap;
    ServerNetworkMessageHandler          m_MessageHandler;
    std::vector<int>                     m_ClientSocketVector;
};

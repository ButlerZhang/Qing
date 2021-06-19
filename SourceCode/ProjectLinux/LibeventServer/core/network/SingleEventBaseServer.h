#pragma once
#include "UDPBroadcast.h"
#include "NetworkBase.h"



class SingleEventBaseServer : public NetworkBase
{
public:

    SingleEventBaseServer();
    virtual ~SingleEventBaseServer();

    virtual bool Start();
    virtual bool Stop();

protected:

    virtual bool ProcessCheckout() { return false; }
    virtual bool ProcessConnected() { return false; }
    virtual bool ProcessDisconnected() { return false; }
    virtual bool ProcessThreadNoticeQueue() { return false; }
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg);

    bool AddThreadNoticeQueueEvent();
    bool DeleteSocket(int ClientSocket);
    bool CreateListener(const std::string &IP, int Port);
    bool HasClient() const { return !m_ClientMap.empty(); }

    bool Send(const void *Data, size_t Size);
    bool Send(const NetworkMessage &NetworkMsg, const void *Data, size_t Size);

private:

    static void CallBack_Listen(evconnlistener *Listener, int Socket, sockaddr *Address, int SocketLen, void *UserData);
    static void CallBack_ThreadNoticeQueue(int Socket, short Events, void *UserData);
    static void CallBack_Event(struct bufferevent *bev, short Events, void *UserData);
    static void CallBack_Recv(struct bufferevent *bev, void *UserData);
    static void CallBack_Send(struct bufferevent *bev, void *UserData);

protected:

    int                                          m_ListenPort;
    std::string                                  m_BindIP;
    EventListener                                m_Listener;
    EventNormal                                  m_NoticeQueueEvent;
    UDPBroadcast                                 m_UDPBroadcast;
    std::map<int, ClientNode>                    m_ClientMap;
};

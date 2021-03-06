#pragma once
#include "NetworkBase.h"



class SingleEventBaseClient : public NetworkBase
{
public:

    SingleEventBaseClient(long ClientID);
    virtual ~SingleEventBaseClient();

    virtual bool Start(const std::string &ServerIP, int Port);
    virtual bool Start(int UDPBroadcastPort);

    inline long GetClientID() const { return m_ClientID; }
    inline bool IsConnected() const { return m_IsConnected; }

protected:

    virtual bool ProcessCheckout();
    virtual bool ProcessConnected() { return false; }
    virtual bool ProcessDisconnected() { return false; }

    bool Send(const void *Data, size_t Size);

    bool AddEventInputFromCMD();
    bool AddEventRecvUDPBroadcast();
    bool ConnectServer(const std::string &ServerIP, int Port);

private:

    static void CallBack_InputFromCMD(int Input, short Events, void *UserData);
    static void CallBack_RecvUDPBroadcast(int Socket, short Events, void *UserData);
    static void CallBack_ClientEvent(struct bufferevent *bev, short Events, void *UserData);
    static void CallBack_RecvFromServer(struct bufferevent *bev, void *UserData);

private:

    bool                                         m_IsConnected;
    long                                         m_ClientID;            //process id
    int                                          m_ServerPort;
    int                                          m_BroadcastPort;
    int                                          m_UDPSocket;
    std::string                                  m_ServerIP;
    struct sockaddr_in                           m_BroadcastAddress;

    EventNormal                                  m_CMDInputEvent;
    std::shared_ptr<EventNormal>                 m_UDPBroadcastEvent;
    std::shared_ptr<EventIOBuffer>               m_IOBuffer;
    std::shared_ptr<EventDataBuffer>             m_RecvDataBuffer;
};

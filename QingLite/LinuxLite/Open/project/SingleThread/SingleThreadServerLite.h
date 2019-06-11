#pragma once
#include "HTTPServer.h"
#include "UDPBroadcast.h"
#include "MessageHandler.h"
#include <string>
#include <vector>
#include <event2/listener.h>



class SingleThreadServerLite
{
public:

    SingleThreadServerLite();
    ~SingleThreadServerLite();

    bool Initialize(const std::string &IP, int Port);
    bool Start();
    bool Stop();

private:

    static void CallBack_Listen(evconnlistener *Listener, int Socket, sockaddr *Address, int SocketLen, void *UserData);
    static void CallBack_Event(struct bufferevent *bev, short Events, void *UserData);
    static void CallBack_Recv(struct bufferevent *bev, void *UserData);
    static void CallBack_Send(struct bufferevent *bev, void *UserData);

private:

    int                              m_ListenPort;
    std::string                      m_ListenIP;
    event_base                      *m_EventBase;
    evconnlistener                  *m_Listener;
    HTTPServer                       m_HTTPServer;
    UDPBroadcast                     m_UDPBroadcast;
    MessageHandler                   m_MessageHandler;
    std::vector<int>                 m_ClientSocketVector;
};

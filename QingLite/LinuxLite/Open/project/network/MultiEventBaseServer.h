#pragma once
#include "NetworkBase.h"
#include "UDPBroadcast.h"
#include <event2/listener.h>



class MultiEventBaseServer
{
public:

    MultiEventBaseServer();
    ~MultiEventBaseServer();

    bool Start(const std::string &IP, int Port, int ThreadCount = 0);
    void Stop();

    virtual bool ProcessConnected(ConnectNode &ConnectedNode);
    virtual bool ProcessDisconnected(ConnectNode &ConnectedNode, short events);
    virtual bool ProcessMessage(ConnectNode &ConnectedNode) { return false; }

protected:

    bool Send(ConnectNode &ConnectedNode, const std::string &DataString);

private:

    bool ProcessRecv(ConnectNode &ConnectedNode);
    bool ProcessSend(ConnectNode &ConnectedNode);

    bool CreateThreads(int ThreadCount);
    bool StartListen(const std::string &IP, int Port);
    bool StartEventLoop(evconnlistener *Listener);

    static void CallBack_Listen(evconnlistener *listener, int Socket, sockaddr *sa, int socklen, void *user_data);
    static void CallBack_Accept(int fd, short which, void *arg);
    static void CallBack_Recv(struct bufferevent *bev, void *data);
    static void CallBack_Send(struct bufferevent *bev, void *data);
    static void CallBack_Event(struct bufferevent *bev, short events, void *data);
    static void* CallBack_StartThreadEventLoop(void *arg);

private:

    ThreadNode                 m_MainThread;
    UDPBroadcast               m_UDPBroadcast;
    std::vector<ThreadNode>    m_ThreadVector;
};

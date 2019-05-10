#pragma once
#include "ThreadNode.h"
#include "TimerEvent.h"
#include "SignalEvent.h"

#include <event2/listener.h>
#include <string>



class MultiThreadServerLite
{
public:

    MultiThreadServerLite();
    ~MultiThreadServerLite();

    bool StartServer(const std::string &IP, int Port, int ThreadCount = 0);
    void StopServer();

    virtual bool ProcessConnect(ConnectNode &ConnectedNode);
    virtual bool ProcessRecv(ConnectNode &ConnectedNode);
    virtual bool ProcessSend(ConnectNode &ConnectedNode);
    virtual bool ProcessClose(ConnectNode &ConnectedNode, short events);

    inline TimerEvent& GetTimerEvent() { return m_TimerEvent; }
    inline SignalEvent& GetSignalEvent() { return m_SignalEvent; }

private:

    bool CreateThreads(int ThreadCount);
    bool StartListen(const std::string &IP, int Port);
    bool StartEventLoop(evconnlistener *Listener);

private:

    static void CallBack_Listen(evconnlistener *listener, evutil_socket_t Socket, sockaddr *sa, int socklen, void *user_data);
    static void CallBack_Accept(int fd, short which, void *arg);
    static void CallBack_Recv(struct bufferevent *bev, void *data);
    static void CallBack_Send(struct bufferevent *bev, void *data);
    static void CallBack_Event(struct bufferevent *bev, short events, void *data);
    static void* CallBack_StartThreadEventLoop(void *arg);

private:

    ThreadNode                 m_MainThread;
    TimerEvent                 m_TimerEvent;
    SignalEvent                m_SignalEvent;
    std::vector<ThreadNode>    m_ThreadVector;
};

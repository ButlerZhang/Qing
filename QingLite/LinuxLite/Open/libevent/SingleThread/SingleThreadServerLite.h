#pragma once
#include <string>
#include <vector>
#include <event.h>
#include <event2/listener.h>



class SingleThreadServerLite
{
public:

    SingleThreadServerLite();
    ~SingleThreadServerLite();

    bool StartServer(const std::string &IP, int Port);

private:

    static void CallBack_Listen(evconnlistener *Listener, evutil_socket_t Socket, sockaddr *sa, int socklen, void *UserData);
    static void CallBack_Event(struct bufferevent *bev, short events, void *UserData);
    static void CallBack_Recv(struct bufferevent *bev, void *UserData);
    static void CallBack_Send(struct bufferevent *bev, void *UserData);

private:

    event_base                  *m_base;
    std::vector<int>             m_ClientSocketVector;
};

#include "SingleThreadServerLite.h"
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <algorithm>
#include <event2/bufferevent.h>



SingleThreadServerLite::SingleThreadServerLite()
{
    m_EventBase = NULL;
    m_Listener = NULL;
}

SingleThreadServerLite::~SingleThreadServerLite()
{
    m_ClientSocketVector.clear();

    evconnlistener_free(m_Listener);
    event_base_free(m_EventBase);
}

bool SingleThreadServerLite::Initialize(const std::string &IP, int Port)
{
    if (m_EventBase != NULL && m_Listener != NULL)
    {
        printf("Re-create event base.\n");
        return true;
    }

    m_EventBase = event_base_new();
    if (m_EventBase == NULL)
    {
        printf("Create main event base error.\n");
        return false;
    }

    sockaddr_in BindAddress;
    bzero(&BindAddress, sizeof(sockaddr_in));
    BindAddress.sin_family = AF_INET;
    BindAddress.sin_port = htons(static_cast<uint16_t>(Port));
    if (!IP.empty())
    {
        inet_pton(AF_INET, IP.c_str(), &(BindAddress.sin_addr));
    }

    m_Listener = evconnlistener_new_bind(
        m_EventBase,
        CallBack_Listen,
        (void*)this,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        5,
        (sockaddr*)&BindAddress,
        sizeof(sockaddr_in));

    if (m_Listener == NULL)
    {
        printf("Create listener error.\n");
        return false;
    }

    return true;
}

bool SingleThreadServerLite::Start()
{
    if (!m_UDPBroadcast.BindEventBase(m_EventBase))
    {
        printf("UDP bind event base error.\n");
        return false;
    }

    m_UDPBroadcast.StartTimer(10);

    printf("Server start dispatch...\n\n");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleThreadServerLite::Stop()
{
    return false;
}

void SingleThreadServerLite::CallBack_Listen(evconnlistener * Listener, evutil_socket_t Socket, sockaddr *sa, int socklen, void *UserData)
{
    SingleThreadServerLite *Server = (SingleThreadServerLite*)UserData;
    Server->m_ClientSocketVector.push_back(Socket);
    printf("Accept client, socket = %d, socket count = %d.\n\n", Socket, static_cast<int>(Server->m_ClientSocketVector.size()));

    bufferevent *bev = bufferevent_socket_new(Server->m_EventBase, Socket, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, CallBack_Recv, CallBack_Send, CallBack_Event, Server);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
    bufferevent_enable(bev, EV_WRITE | EV_PERSIST);
}

void SingleThreadServerLite::CallBack_Event(bufferevent * bev, short events, void *UserData)
{
    int ClientSocket = bufferevent_getfd(bev);
    SingleThreadServerLite *Server = (SingleThreadServerLite*)UserData;
    std::remove(Server->m_ClientSocketVector.begin(), Server->m_ClientSocketVector.end(), ClientSocket);

    if (events & BEV_EVENT_EOF)
    {
        printf("Client = %d connection closed.\n\n", ClientSocket);
    }
    else if (events & BEV_EVENT_ERROR)
    {
        printf("Client = %d unknow error.\n\n", ClientSocket);
    }

    bufferevent_free(bev);
}

void SingleThreadServerLite::CallBack_Recv(bufferevent * bev, void *UserData)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    size_t RecvSize = bufferevent_read(bev, Message, sizeof(Message));
    Message[RecvSize] = '\0';

    int ClientSocket = bufferevent_getfd(bev);
    printf("Recv, client = %d, message = %s, size = %d.\n", ClientSocket, Message, RecvSize);

    const std::string ACK("ACK");
    bufferevent_write(bev, ACK.c_str(), ACK.length());
    printf("Client = %d send ack, size = %d.\n\n", ClientSocket, ACK.length());
}

void SingleThreadServerLite::CallBack_Send(bufferevent * bev, void * UserData)
{
    //evbuffer *WriteBuffer = bufferevent_get_output(bev);
    //if (evbuffer_get_length(WriteBuffer) <= 0)
    //{
    //    printf("Process send, no data.\n\n");
    //    return;
    //}

    //int ClientSocket = bufferevent_getfd(bev);
    //printf("Client = %d send data.\n", ClientSocket);
}

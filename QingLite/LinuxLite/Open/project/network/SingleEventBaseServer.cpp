#include "SingleEventBaseServer.h"
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <algorithm>
#include <event2/bufferevent.h>



SingleEventBaseServer::SingleEventBaseServer()
{
    m_Listener = NULL;
    m_ListenPort = 0;
    m_EventBase = event_base_new();
}

SingleEventBaseServer::~SingleEventBaseServer()
{
    m_ClientSocketVector.clear();

    evconnlistener_free(m_Listener);
    event_base_free(m_EventBase);
}

bool SingleEventBaseServer::Start(const std::string &IP, int Port)
{
    if (!CreateListener(IP, Port))
    {
        return false;
    }

    if (!m_UDPBroadcast.BindBaseEvent(m_EventBase))
    {
        printf("ERROR: UDP braodcast bind event base failed.\n");
        return false;
    }

    if (!m_HTTPServer.BindBaseEvent(m_EventBase))
    {
        printf("ERROR: HTTP server bind event base failed.\n");
        return false;
    }

    if (!m_HTTPServer.Start(m_ListenIP, m_ListenPort + 1))
    {
        printf("ERROR: http server start failed.\n");
        return false;
    }

    if (!m_MessageHandler.Start(this))
    {
        printf("ERROR: Message handler start failed.\n");
        return false;
    }

    m_UDPBroadcast.StartTimer(m_ListenIP, 10, m_ListenPort);

    printf("Server start dispatch...\n");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleEventBaseServer::Stop()
{
    return event_base_loopbreak(m_EventBase) == 0;
}

bool SingleEventBaseServer::ProcessMessage(NetworkMessage &NetworkMsg)
{
    std::string ACK("Client=");
    ACK += std::to_string(NetworkMsg.m_Socket);
    ACK += std::string(", ACK=") + NetworkMsg.m_Message;

    if (bufferevent_write(NetworkMsg.m_Bufferevent, ACK.c_str(), ACK.length()) != 0)
    {
        printf("Send failed, %s\n", ACK.c_str());
        return false;
    }
    else
    {
        printf("%s\n", ACK.c_str());
        return true;
    }
}

bool SingleEventBaseServer::CreateListener(const std::string &IP, int Port)
{
    if (m_EventBase == NULL)
    {
        printf("ERROR: Create event base error.\n");
        return false;
    }

    if (m_Listener != NULL)
    {
        printf("ERROR: Re-create event base.\n");
        return true;
    }

    sockaddr_in BindAddress;
    bzero(&BindAddress, sizeof(sockaddr_in));
    BindAddress.sin_family = AF_INET;
    BindAddress.sin_port = htons(static_cast<uint16_t>(Port));
    if (!IP.empty())
    {
        inet_pton(AF_INET, IP.c_str(), &(BindAddress.sin_addr));
        printf("Server bind IP = %s, port = %d.\n", IP.c_str(), Port);
    }
    else
    {
        BindAddress.sin_addr.s_addr = INADDR_ANY;
        printf("Server bind any IP, port = %d.\n", Port);
    }

    m_Listener = evconnlistener_new_bind(
        m_EventBase,
        CallBack_Listen,
        this,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        10,
        (sockaddr*)&BindAddress,
        sizeof(sockaddr_in));

    if (m_Listener == NULL)
    {
        printf("ERROR: Create listener failed.\n");
        return false;
    }

    m_ListenIP = IP;
    m_ListenPort = Port;
    return true;
}

bool SingleEventBaseServer::Send(const NetworkMessage &NetworkMsg, const void *Data, size_t Size)
{
    if (NetworkMsg.m_Bufferevent == NULL)
    {
        return false;
    }

    if (bufferevent_write(NetworkMsg.m_Bufferevent, Data, Size) != 0)
    {
        printf("Send failed.\n");
        return false;
    }

    printf("Send succeed.\n");
    return true;
}

void SingleEventBaseServer::CallBack_Listen(evconnlistener * Listener, int Socket, sockaddr *sa, int socklen, void *UserData)
{
    SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;
    Server->m_ClientSocketVector.push_back(Socket);
    printf("Accept client, socket = %d, current count = %d.\n", Socket, static_cast<int>(Server->m_ClientSocketVector.size()));

    bufferevent *bev = bufferevent_socket_new(Server->m_EventBase, Socket, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, CallBack_Recv, CallBack_Send, CallBack_Event, Server);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
    bufferevent_enable(bev, EV_WRITE | EV_PERSIST);
}

void SingleEventBaseServer::CallBack_Event(bufferevent * bev, short Events, void *UserData)
{
    bool IsAllowDelete = false;
    int ClientSocket = bufferevent_getfd(bev);

    if (Events & BEV_EVENT_EOF)
    {
        IsAllowDelete = true;
        printf("Client = %d connection closed.\n", ClientSocket);
    }
    else if (Events & BEV_EVENT_TIMEOUT)
    {
        printf("Client = %d user specified timeout reached.\n", ClientSocket);
    }
    else if (Events & BEV_EVENT_CONNECTED)
    {
        printf("Client = %d connect operation finished.\n", ClientSocket);
    }
    else
    {
        IsAllowDelete = true;
        printf("ERROR: Client = %d unknow error.\n", ClientSocket);
    }

    if (IsAllowDelete)
    {
        SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;
        std::vector<int>::iterator it = std::find(Server->m_ClientSocketVector.begin(), Server->m_ClientSocketVector.end(), ClientSocket);
        if (it != Server->m_ClientSocketVector.end())
        {
            Server->m_ClientSocketVector.erase(it);
            printf("Delete client = %d, surplus count = %d.\n", ClientSocket, static_cast<int>(Server->m_ClientSocketVector.size()));
        }

        bufferevent_free(bev);
    }
}

void SingleEventBaseServer::CallBack_Recv(bufferevent *bev, void *UserData)
{
    char ClientMessage[NETWORK_BUFFER_SIZE];
    memset(ClientMessage, 0, sizeof(ClientMessage));

    int ClientSocket = bufferevent_getfd(bev);
    size_t RecvSize = bufferevent_read(bev, ClientMessage, sizeof(ClientMessage));
    printf("Recv client = %d message size = %d\n", ClientSocket, RecvSize);

    if (RecvSize > 0)
    {
        NetworkMessage NetworkMsg;
        NetworkMsg.m_Bufferevent = bev;
        NetworkMsg.m_Socket = bufferevent_getfd(bev);
        NetworkMsg.m_Message.assign(ClientMessage, ClientMessage + RecvSize);

        SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;
        Server->m_MessageHandler.PushMessage(NetworkMsg);
    }
}

void SingleEventBaseServer::CallBack_Send(bufferevent * bev, void * UserData)
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

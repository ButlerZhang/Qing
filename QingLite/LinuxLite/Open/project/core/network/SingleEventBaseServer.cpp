#include "SingleEventBaseServer.h"
#include "../tools/BoostLog.h"
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <algorithm>
#include <event2/bufferevent.h>



SingleEventBaseServer::SingleEventBaseServer()
{
    m_Listener = NULL;
    m_CheckoutTimer = NULL;

    m_ListenPort = 0;
    m_EventBase = event_base_new();
}

SingleEventBaseServer::~SingleEventBaseServer()
{
    if (event_base_loopbreak(m_EventBase) != 0)
    {
        g_Log.WriteError("Single base server event can not loop break.");
    }

    for (auto it = m_ClientSocketMap.begin(); it != m_ClientSocketMap.end(); it++)
    {
        evutil_closesocket(it->first);
        evbuffer_free(it->second);
    }

    m_ClientSocketMap.clear();
    if (m_CheckoutTimer != NULL)
    {
        event_free(m_CheckoutTimer);
        m_CheckoutTimer = NULL;
    }

    if (m_Listener != NULL)
    {
        evconnlistener_free(m_Listener);
        m_Listener = NULL;
    }

    event_base_free(m_EventBase);
    m_EventBase = NULL;
    g_Log.WriteDebug("Single base server is release.");
}

bool SingleEventBaseServer::Start(const std::string &IP, int Port)
{
    if (!CreateListener(IP, Port))
    {
        return false;
    }

    if (!m_SignalEventMap.BindBaseEvent(m_EventBase) || !m_SignalEventMap.AddSignalEvent(SIGINT, CallBack_Signal, this))
    {
        g_Log.WriteError("Signal event map bind or add error.");
        return false;
    }

    if (!m_UDPBroadcast.BindBaseEvent(m_EventBase))
    {
        g_Log.WriteError("UDP braodcast bind event base failed.");
        return false;
    }

    if (!m_MessageHandler.Start(this))
    {
        g_Log.WriteError("Message handler start failed.");
        return false;
    }

    if (!AddCheckoutTimer(5))
    {
        g_Log.WriteError("Add single server checkout timer failed.");
        return false;
    }

    m_UDPBroadcast.StartTimer(m_ListenIP, 10, m_ListenPort);

    g_Log.WriteInfo(BoostFormat("Single Server(%s:%d) start dispatch...", m_ListenIP.c_str(), m_ListenPort));
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleEventBaseServer::Stop()
{
    m_MessageHandler.Stop();
    if (event_base_loopbreak(m_EventBase) == 0)
    {
        g_Log.WriteInfo("Single Server loop break.");
        return true;
    }

    g_Log.WriteError("Single Server loop break failed.");
    return false;
}

bool SingleEventBaseServer::ProcessMessage(NetworkMessage &NetworkMsg)
{
    std::string ACK("Client=");
    ACK += std::to_string(NetworkMsg.m_Socket);
    ACK += std::string(", ACK=") + NetworkMsg.m_Message;

    if (bufferevent_write(NetworkMsg.m_Bufferevent, ACK.c_str(), ACK.length()) != 0)
    {
        g_Log.WriteError(BoostFormat("Send failed, %s", ACK.c_str()));
        return false;
    }
    else
    {
        g_Log.WriteDebug(BoostFormat("%s", ACK.c_str()));
        return true;
    }
}

bool SingleEventBaseServer::DeleteSocket(int ClientSocket)
{
    std::map<int, evbuffer*>::iterator it = m_ClientSocketMap.find(ClientSocket);
    if (it == m_ClientSocketMap.end())
    {
        g_Log.WriteError(BoostFormat("Delete socket, can not find socket = %d.", ClientSocket));
        return false;
    }

    if (evutil_closesocket(ClientSocket) != 0)
    {
        g_Log.WriteError(BoostFormat("Delete socket, can not close socket = %d.", ClientSocket));
        return false;
    }

    evbuffer_free(it->second);
    m_ClientSocketMap.erase(it);
    int ClientCount = static_cast<int>(m_ClientSocketMap.size());
    g_Log.WriteError(BoostFormat("Delete socket = %d, surplus client count = %d.",ClientSocket, ClientCount));

    return true;
}

bool SingleEventBaseServer::AddCheckoutTimer(int TimerInternal)
{
    if (m_CheckoutTimer != NULL)
    {
        g_Log.WriteError("Re-create signle server checkout timer.");
        return true;
    }

    m_CheckoutTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_Checkout, this);
    if (m_CheckoutTimer == NULL)
    {
        g_Log.WriteError("Create signle server chekcout timer failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = TimerInternal;

    if (event_add(m_CheckoutTimer, &tv) != 0)
    {
        g_Log.WriteError("Add signle server checkout timer failed.");
        event_free(m_CheckoutTimer);
        m_CheckoutTimer = NULL;
        return false;
    }

    return true;
}

bool SingleEventBaseServer::CreateListener(const std::string &IP, int Port)
{
    if (m_EventBase == NULL)
    {
        g_Log.WriteError("Create event base error.");
        return false;
    }

    if (m_Listener != NULL)
    {
        g_Log.WriteError("Re-create event base.");
        return true;
    }

    sockaddr_in BindAddress;
    bzero(&BindAddress, sizeof(sockaddr_in));
    BindAddress.sin_family = AF_INET;
    BindAddress.sin_port = htons(static_cast<uint16_t>(Port));
    if (!IP.empty())
    {
        inet_pton(AF_INET, IP.c_str(), &(BindAddress.sin_addr));
        g_Log.WriteDebug(BoostFormat("Server bind IP = %s, port = %d.", IP.c_str(), Port));
    }
    else
    {
        BindAddress.sin_addr.s_addr = INADDR_ANY;
        g_Log.WriteDebug(BoostFormat("Server bind any IP, port = %d.", Port));
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
        g_Log.WriteInfo("Create listener failed.");
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
        g_Log.WriteError(BoostFormat("Client = %d bufferevent is NULL.", NetworkMsg.m_Socket));
        return false;
    }

    if (bufferevent_write(NetworkMsg.m_Bufferevent, Data, Size) != 0)
    {
        g_Log.WriteError(BoostFormat("Client = %d send failed.", NetworkMsg.m_Socket));
        return false;
    }

    g_Log.WriteError(BoostFormat("Client = %d send size = %d succeed.", NetworkMsg.m_Socket, Size));
    return true;
}

void SingleEventBaseServer::CallBack_Listen(evconnlistener *Listener, int Socket, sockaddr *sa, int socklen, void *UserData)
{
    SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;
    bufferevent *bev = bufferevent_socket_new(Server->m_EventBase, Socket, BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL)
    {
        g_Log.WriteError(BoostFormat("Listen: bufferevent_socket_new error, socket = %d.", Socket));
        evutil_closesocket(Socket);
        return;
    }

    struct evbuffer *ClientBuffer = evbuffer_new();
    if (ClientBuffer == NULL)
    {
        g_Log.WriteError(BoostFormat("Listen: evbuffer_new error, socket = %d.", Socket));
        evutil_closesocket(Socket);
        return;
    }

    Server->m_ClientSocketMap[Socket] = ClientBuffer;
    int ClientCount = static_cast<int>(Server->m_ClientSocketMap.size());
    g_Log.WriteInfo(BoostFormat("Listen: Accept client, socket = %d, client count = %d.", Socket, ClientCount));

    bufferevent_setcb(bev, CallBack_Recv, CallBack_Send, CallBack_Event, Server);
    bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
    //bufferevent_setwatermark(bev, EV_READ, 0, 0);

    size_t lowmark = 0, highmark = 0;
    bufferevent_getwatermark(bev, EV_READ, &lowmark, &highmark);
    g_Log.WriteDebug(BoostFormat("Listen: default read water low mark = %d, high mark = %d.", lowmark, highmark));

    bufferevent_getwatermark(bev, EV_WRITE, &lowmark, &highmark);
    g_Log.WriteDebug(BoostFormat("Listen: default write water low mark = %d, high mark = %d.", lowmark, highmark));
}

void SingleEventBaseServer::CallBack_Checkout(int Socket, short Events, void *UserData)
{
    SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;
    Server->ProcessCheckout();

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 5;          //TimerInternal
    event_add(Server->m_CheckoutTimer, &tv);
}

void SingleEventBaseServer::CallBack_Signal(int Signal, short Events, void *UserData)
{
    g_Log.WriteDebug("Single server caught an interrupt signal; exiting cleanly in one seconds.");

    SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;

    struct timeval delay = { 1, 0 };
    event_base_loopexit(Server->m_EventBase, &delay);
}

void SingleEventBaseServer::CallBack_Event(bufferevent * bev, short Events, void *UserData)
{
    bool IsAllowDelete = false;
    int ClientSocket = bufferevent_getfd(bev);

    if (Events & BEV_EVENT_EOF)
    {
        IsAllowDelete = true;
        g_Log.WriteError(BoostFormat("Client = %d connection closed.", ClientSocket));
    }
    else if (Events & BEV_EVENT_TIMEOUT)
    {
        g_Log.WriteError(BoostFormat("Client = %d user specified timeout reached.", ClientSocket));
    }
    else if (Events & BEV_EVENT_CONNECTED)
    {
        g_Log.WriteInfo(BoostFormat("Client = %d connect operation finished.", ClientSocket));
    }
    else
    {
        IsAllowDelete = true;
        g_Log.WriteError(BoostFormat("Client = %d unknow error.", ClientSocket));
    }

    if (IsAllowDelete)
    {
        SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;
        if (Server->DeleteSocket(ClientSocket))
        {
            bufferevent_free(bev);
        }
    }
}

void SingleEventBaseServer::CallBack_Recv(bufferevent *bev, void *UserData)
{
    SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;
    g_Log.WriteDebug("Process recv call back.");

    int ClientSocket = bufferevent_getfd(bev);
    if (Server->m_ClientSocketMap.find(ClientSocket) == Server->m_ClientSocketMap.end())
    {
        g_Log.WriteError(BoostFormat("Recv can not find socket = %d.", ClientSocket));
        return;
    }

    const size_t MESSAGE_HEADER_LENGTH_SIZE = 4;
    std::vector<char> MessageHeaderLengthBuffer(MESSAGE_HEADER_LENGTH_SIZE, 0);

    std::vector<char> RecvBuffer(NETWORK_BUFFER_SIZE, 0);
    struct evbuffer *EventBuffer = Server->m_ClientSocketMap[ClientSocket];

    size_t RecvSize = 0, EventBufferLength = 0;
    while (RecvSize = bufferevent_read(bev, &RecvBuffer[0], RecvBuffer.size()), RecvSize > 0)
    {
        g_Log.WriteDebug(BoostFormat("Recv client = %d message size = %llu", ClientSocket, RecvSize));

        if (evbuffer_add(EventBuffer, &RecvBuffer[0], RecvSize) != 0)
        {
            g_Log.WriteError(BoostFormat("Recv message can not add to evbuffer, client = %d.", ClientSocket));
            continue;
        }

        while (EventBufferLength = evbuffer_get_length(EventBuffer), EventBufferLength > 0)
        {
            if (EventBufferLength <= MESSAGE_HEADER_LENGTH_SIZE)
            {
                g_Log.WriteError(BoostFormat("Recv message length is only %llu bytes.", EventBufferLength));
                break;
            }

            if (evbuffer_copyout(EventBuffer, &MessageHeaderLengthBuffer[0], MESSAGE_HEADER_LENGTH_SIZE) != MESSAGE_HEADER_LENGTH_SIZE)
            {
                g_Log.WriteError("evbuffer_copyout can not read message length.");
                break;
            }

            int MessageBodyLength = 0;
            ::memcpy(&MessageBodyLength, &MessageHeaderLengthBuffer[0], sizeof(int));
            size_t MessageTotalLength = ::ntohl(MessageBodyLength) + MESSAGE_HEADER_LENGTH_SIZE;

            g_Log.WriteDebug(BoostFormat("Recv message total length = %llu, event buffer length = %llu.", MessageTotalLength, EventBufferLength));
            if (EventBufferLength < MessageTotalLength)
            {
                break;
            }

            if (MessageTotalLength > RecvBuffer.size())
            {
                g_Log.WriteDebug(BoostFormat("Recv buffer old size = %llu, new size = %llu.", RecvBuffer.size(), MessageTotalLength));
                RecvBuffer.resize(MessageTotalLength, 0);
            }

            if (evbuffer_remove(EventBuffer, &RecvBuffer[0], MessageTotalLength) != static_cast<int>(MessageTotalLength))
            {
                g_Log.WriteError(BoostFormat("evbuffer_remove can not remove size = %llu data.", MessageTotalLength));
                break;
            }

            NetworkMessage NetworkMsg;
            NetworkMsg.m_Bufferevent = bev;
            NetworkMsg.m_Socket = ClientSocket;
            NetworkMsg.m_Message.assign(RecvBuffer.begin(), RecvBuffer.begin() + MessageTotalLength);
            Server->m_MessageHandler.PushMessage(NetworkMsg);
        }
    }
}

void SingleEventBaseServer::CallBack_Send(bufferevent * bev, void * UserData)
{
    //evbuffer *WriteBuffer = bufferevent_get_output(bev);
    //if (evbuffer_get_length(WriteBuffer) <= 0)
    //{
    //    g_Log.WriteError("Process send, no data.");
    //    return;
    //}

    //int ClientSocket = bufferevent_getfd(bev);
    //g_Log.WriteInfo(BoostFormat("Client = %d send data.", ClientSocket));
}

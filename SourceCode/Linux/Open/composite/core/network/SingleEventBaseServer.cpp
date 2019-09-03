#include "SingleEventBaseServer.h"
#include "../../../../../Common/Boost/BoostLog.h"
#include "ThreadNoticeQueue.h"



SingleEventBaseServer::SingleEventBaseServer()
{
    m_ListenPort = 0;
}

SingleEventBaseServer::~SingleEventBaseServer()
{
    Stop();
    m_ClientMap.clear();
    g_Log.WriteDebug("Single base server was destructored.");
}

bool SingleEventBaseServer::Start()
{
    if (!AddCheckoutTimer(1) || !AddThreadNoticeQueueEvent() || !AddSignal())
    {
        return false;
    }

    g_Log.WriteInfo("Single Server start dispatch...");
    event_base_dispatch(m_EventBase.m_eventbase);
    return true;
}

bool SingleEventBaseServer::Stop()
{
    if (event_base_loopbreak(m_EventBase.m_eventbase) == 0)
    {
        g_Log.WriteDebug("Single base server loop break.");
        return true;
    }

    g_Log.WriteError("Single base server loop break failed.");
    return false;
}

bool SingleEventBaseServer::ProcessMessage(NetworkMessage &NetworkMsg)
{
    const std::string &ACK = "Client=" + std::to_string(NetworkMsg.m_Socket) + ", ACK=" + NetworkMsg.m_Message;
    return Send(NetworkMsg, ACK.c_str(), ACK.size());
}

bool SingleEventBaseServer::AddThreadNoticeQueueEvent()
{
    if (m_NoticeQueueEvent.m_event != NULL)
    {
        g_Log.WriteError("Single base server re-create thread notice queue event.");
        return true;
    }

    m_NoticeQueueEvent.m_event = event_new(m_EventBase.m_eventbase, g_ThreadNoticeQueue.GetRecvDescriptor(), EV_READ | EV_PERSIST, CallBack_ThreadNoticeQueue, this);
    if (m_NoticeQueueEvent.m_event == NULL)
    {
        g_Log.WriteError("Single base server create thread notice queue event failed.");
        return false;
    }

    if (event_add(m_NoticeQueueEvent.m_event, NULL) != 0)
    {
        g_Log.WriteError("Single base server add thread notice queue event failed.");
        return false;
    }

    return true;
}

bool SingleEventBaseServer::DeleteSocket(int ClientSocket)
{
    std::map<int, ClientNode>::iterator it = m_ClientMap.find(ClientSocket);
    if (it == m_ClientMap.end())
    {
        g_Log.WriteError(BoostFormat("Single base server can not find socket = %d to delete.", ClientSocket));
        return false;
    }

    if (evutil_closesocket(ClientSocket) != 0)
    {
        g_Log.WriteError(BoostFormat("Single base server can not close socket = %d.", ClientSocket));
        return false;
    }

    m_ClientMap.erase(it);
    g_Log.WriteError(BoostFormat("Single base server delete socket = %d, surplus client count = %d.", ClientSocket, m_ClientMap.size()));
    return true;
}

bool SingleEventBaseServer::CreateListener(const std::string &IP, int Port)
{
    if (m_EventBase.m_eventbase == NULL)
    {
        g_Log.WriteError("Single base server event base is NULL.");
        return false;
    }

    if (m_Listener.m_listener != NULL)
    {
        g_Log.WriteError("Single base server re-create listener.");
        return true;
    }

    sockaddr_in BindAddress;
    InitializeSocketAddress(BindAddress, IP, Port);

    m_Listener.m_listener = evconnlistener_new_bind(
        m_EventBase.m_eventbase,
        CallBack_Listen,
        this,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC,
        10,
        (sockaddr*)&BindAddress,
        sizeof(sockaddr_in));

    if (m_Listener.m_listener == NULL)
    {
        g_Log.WriteError("Single base server create listener failed.");
        return false;
    }

    m_BindIP = IP;
    m_ListenPort = Port;
    return true;
}

bool SingleEventBaseServer::Send(const void * Data, size_t Size)
{
    int SendCount = 0;
    for (std::map<int, ClientNode>::iterator it = m_ClientMap.begin(); it != m_ClientMap.end(); it++)
    {
        if (it->first < 0 || it->second.m_IOBuffer->m_bufferevent == NULL || it->second.m_DataBuffer->m_evbuffer == NULL)
        {
            continue;
        }

        if (bufferevent_write(it->second.m_IOBuffer->m_bufferevent, Data, Size) != 0)
        {
            g_Log.WriteError(BoostFormat("Single base server client = %d broadcast send failed.", it->first));
            continue;
        }

        ++SendCount;
    }

    g_Log.WriteDebug(BoostFormat("Single base server broadcast, succeed = %d, failed = %d", SendCount, m_ClientMap.size() - SendCount));
    return true;
}

bool SingleEventBaseServer::Send(const NetworkMessage &NetworkMsg, const void *Data, size_t Size)
{
    if (NetworkMsg.m_IOBuffer->m_bufferevent == NULL)
    {
        g_Log.WriteError(BoostFormat("Single base server client = %d bufferevent is NULL.", NetworkMsg.m_Socket));
        return false;
    }

    if (bufferevent_write(NetworkMsg.m_IOBuffer->m_bufferevent, Data, Size) != 0)
    {
        g_Log.WriteError(BoostFormat("Single base server client = %d send failed.", NetworkMsg.m_Socket));
        return false;
    }

    g_Log.WriteInfo(BoostFormat("Single base server client = %d send size = %d succeed.", NetworkMsg.m_Socket, Size));
    return true;
}

void SingleEventBaseServer::CallBack_Listen(evconnlistener *Listener, int Socket, sockaddr *sa, int socklen, void *UserData)
{
    SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;
    if (Server->m_ClientMap.find(Socket) != Server->m_ClientMap.end())
    {
        g_Log.WriteError(BoostFormat("Single base server socket = %d has existed.", Socket));
        return;
    }

    bufferevent *bev = bufferevent_socket_new(Server->m_EventBase.m_eventbase, Socket, BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL)
    {
        g_Log.WriteError(BoostFormat("Single base server bufferevent_socket_new error, socket = %d.", Socket));
        evutil_closesocket(Socket);
        return;
    }

    Server->m_ClientMap[Socket].m_IOBuffer->m_bufferevent = bev;
    g_Log.WriteInfo(BoostFormat("Single base server accept new client, socket = %d, client count = %llu.", Socket, Server->m_ClientMap.size()));

    bufferevent_setcb(bev, CallBack_Recv, CallBack_Send, CallBack_Event, Server);
    bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
    //bufferevent_setwatermark(bev, EV_READ, 0, 0);

    size_t lowmark = 0, highmark = 0;
    bufferevent_getwatermark(bev, EV_READ, &lowmark, &highmark);
    g_Log.WriteDebug(BoostFormat("Single base server default read water mark, low = %d, high = %d.", lowmark, highmark));

    bufferevent_getwatermark(bev, EV_WRITE, &lowmark, &highmark);
    g_Log.WriteDebug(BoostFormat("Single base server default write water mark, low = %d, high = %d.", lowmark, highmark));
}

void SingleEventBaseServer::CallBack_ThreadNoticeQueue(int Socket, short Events, void * UserData)
{
    SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;
    Server->ProcessThreadNoticeQueue();
}

void SingleEventBaseServer::CallBack_Event(bufferevent *bev, short Events, void *UserData)
{
    bool IsAllowDelete = false;
    int  ClientSocket = bufferevent_getfd(bev);

    if (Events & BEV_EVENT_READING)
    {
        IsAllowDelete = true;
        g_Log.WriteError(BoostFormat("Single base server, client = %d error encountered while reading.", ClientSocket));
    }
    else if (Events & BEV_EVENT_WRITING)
    {
        IsAllowDelete = true;
        g_Log.WriteError(BoostFormat("Single base server, client = %d error encountered while writing.", ClientSocket));
    }
    else if (Events & BEV_EVENT_EOF)
    {
        IsAllowDelete = true;
        g_Log.WriteError(BoostFormat("Single base server, client = %d connection closed.", ClientSocket));
    }
    else if (Events & BEV_EVENT_TIMEOUT)
    {
        g_Log.WriteError(BoostFormat("Single base server, client = %d timeout reached.", ClientSocket));
    }
    else if (Events & BEV_EVENT_CONNECTED)
    {
        g_Log.WriteInfo(BoostFormat("Single base server, client = %d connected succeed.", ClientSocket));
    }
    else if (Events & BEV_EVENT_ERROR)
    {
        IsAllowDelete = true;
        g_Log.WriteError(BoostFormat("Single base server, client = %d unrecoverable error encountered.", ClientSocket));
    }
    else
    {
        IsAllowDelete = true;
        g_Log.WriteError(BoostFormat("Single base server, client = %d unknow error = %d.", ClientSocket, Events));
    }

    if (IsAllowDelete)
    {
        SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;
        Server->DeleteSocket(ClientSocket);
    }
}

void SingleEventBaseServer::CallBack_Recv(bufferevent *bev, void *UserData)
{
    g_Log.WriteDebug("Single base server process recv call back.");
    SingleEventBaseServer *Server = (SingleEventBaseServer*)UserData;

    int ClientSocket = bufferevent_getfd(bev);
    if (Server->m_ClientMap.find(ClientSocket) == Server->m_ClientMap.end())
    {
        g_Log.WriteError(BoostFormat("Single base server recv call back can not find socket = %d.", ClientSocket));
        evutil_closesocket(ClientSocket);
    }
    else
    {
        Server->RecvData(ClientSocket, Server->m_ClientMap[ClientSocket].m_IOBuffer, Server->m_ClientMap[ClientSocket].m_DataBuffer);
    }
}

void SingleEventBaseServer::CallBack_Send(bufferevent *bev, void *UserData)
{
    //TODO
}

#include "SingleEventBaseClient.h"
#include "../../../LinuxTools.h"
#include "../../../Share/Boost/BoostLog.h"
#include "../../../Share/Boost/BoostTools.h"



SingleEventBaseClient::SingleEventBaseClient(long ClientID)
{
    m_ServerPort = 0;
    m_UDPSocket = -1;
    m_BroadcastPort = 0;
    m_IsConnected = false;
    m_ClientID = ClientID;
    m_RecvDataBuffer = std::make_shared<EventDataBuffer>();
}

SingleEventBaseClient::~SingleEventBaseClient()
{
    event_base_loopbreak(m_EventBase.m_eventbase);
    g_Log.WriteDebug(BoostFormat("Single base client = %ld was destructored.", GetClientID()));
}

bool SingleEventBaseClient::Start(const std::string &ServerIP, int Port)
{
    if (m_EventBase.m_eventbase == NULL)
    {
        g_Log.WriteError("Single base client event base is NULL.");
        return false;
    }

    if (m_IsConnected)
    {
        g_Log.WriteError("Single base client re-start.");
        return true;
    }

    if (Port <= 0 || !IsIPValid(ServerIP))
    {
        g_Log.WriteError(BoostFormat("Single base client parameters worng, port = %d, ip = %s", Port, ServerIP.c_str()));
        return false;
    }

    m_ServerPort = Port;
    m_ServerIP = ServerIP;
    if (!AddEventInputFromCMD() || !AddCheckoutTimer(1) || !AddSignal())
    {
        return false;
    }

    g_Log.WriteInfo(BoostFormat("Single base client = %ld start dispatch...", GetClientID()));
    event_base_dispatch(m_EventBase.m_eventbase);
    return true;
}

bool SingleEventBaseClient::Start(int UDPBroadcastPort)
{
    if (m_EventBase.m_eventbase == NULL)
    {
        g_Log.WriteError("Single base client event base is NULL.");
        return false;
    }

    if (m_IsConnected)
    {
        g_Log.WriteError("Single base client re-start.");
        return true;
    }

    if (UDPBroadcastPort <= 0)
    {
        g_Log.WriteError(BoostFormat("Single base client parameters worng, udp broad cast port = %d", UDPBroadcastPort));
        return false;
    }

    m_BroadcastPort = UDPBroadcastPort;
    if (!AddEventInputFromCMD() || !AddEventRecvUDPBroadcast() || !AddCheckoutTimer(1))
    {
        return false;
    }

    g_Log.WriteInfo(BoostFormat("Single base client = %ld start dispatch...", GetClientID()));
    event_base_dispatch(m_EventBase.m_eventbase);
    return true;
}

bool SingleEventBaseClient::Send(const void * Data, size_t Size)
{
    if (!m_IsConnected)
    {
        g_Log.WriteError("Single base client can not send data, no connected.");
        return false;
    }

    if (m_IOBuffer == NULL || m_IOBuffer->m_bufferevent == NULL)
    {
        g_Log.WriteError("Single base client can not send data, IO buffer is NULL.");
        return false;
    }

    if (bufferevent_write(m_IOBuffer->m_bufferevent, Data, Size) != 0)
    {
        g_Log.WriteError(BoostFormat("Single base client send data failed, size = %d.", Size));
        return false;
    }

    static unsigned long long SendCount = 0;
    g_Log.WriteInfo(BoostFormat("Single base client = %ld send succeed, size = %d, count = %llu.", GetClientID(), Size, ++SendCount));
    return true;
}

bool SingleEventBaseClient::ProcessCheckout()
{
    if (m_IsConnected)
    {
        return true;
    }

    if (m_BroadcastPort > 0)
    {
        if (m_UDPBroadcastEvent == NULL)
        {
            g_Log.WriteDebug("Single base client process check out: add recv udp broadcast.");
            AddEventRecvUDPBroadcast();
        }

        return false;
    }

    if (!m_ServerIP.empty() && m_ServerPort > 0)
    {
        g_Log.WriteDebug("Single base client process check out: connect server.");
        ConnectServer(m_ServerIP, m_ServerPort);
        return false;
    }

    //TODO
    return false;
}

bool SingleEventBaseClient::AddEventInputFromCMD()
{
    if (m_CMDInputEvent.m_event != NULL)
    {
        g_Log.WriteError("Single base client re-create CMD event.");
        return true;
    }

    m_CMDInputEvent.m_event = event_new(m_EventBase.m_eventbase, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_InputFromCMD, this);
    if (m_CMDInputEvent.m_event == NULL)
    {
        g_Log.WriteError("Single base client create CMD event failed.");
        return false;
    }

    if (event_add(m_CMDInputEvent.m_event, NULL) != 0)
    {
        g_Log.WriteError("Single base client add CMD event failed.");
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddEventRecvUDPBroadcast()
{
    if (m_UDPSocket > 0)
    {
        close(m_UDPSocket);
        m_UDPSocket = -1;
    }

    m_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_UDPSocket == -1)
    {
        g_Log.WriteError(BoostFormat("Single base client create udp socket error = %s.", strerror(errno)));
        return false;
    }

    int Optval = 1;
    if (setsockopt(m_UDPSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &Optval, sizeof(int)) < 0)
    {
        g_Log.WriteError(BoostFormat("Single base client set udp sockopt error = %s.", strerror(errno)));
        return false;
    }

    InitializeSocketAddress(m_BroadcastAddress, std::string(), m_BroadcastPort);
    if (bind(m_UDPSocket, (struct sockaddr *)&m_BroadcastAddress, sizeof(m_BroadcastAddress)) == -1)
    {
        g_Log.WriteError(BoostFormat("Single base client bind udp socket error = %s.", strerror(errno)));
        return false;
    }

    m_UDPBroadcastEvent = std::make_shared<EventNormal>();
    m_UDPBroadcastEvent->m_event = event_new(m_EventBase.m_eventbase, m_UDPSocket, EV_READ | EV_PERSIST, CallBack_RecvUDPBroadcast, this);
    if (m_UDPBroadcastEvent->m_event == NULL)
    {
        g_Log.WriteError("Single base client create udp broadcast event failed.");
        return false;
    }

    if (event_add(m_UDPBroadcastEvent->m_event, NULL) == -1)
    {
        g_Log.WriteError("Single base client add udp broadcast event failed.");
        return false;
    }

    return true;
}

bool SingleEventBaseClient::ConnectServer(const std::string &ServerIP, int Port)
{
    if (m_IsConnected)
    {
        g_Log.WriteError("Single base client re-connect server.");
        return true;
    }

    struct sockaddr_in ServerAddress;
    InitializeSocketAddress(ServerAddress, ServerIP, Port);

    m_IOBuffer = std::make_shared<EventIOBuffer>();
    m_IOBuffer->m_bufferevent = bufferevent_socket_new(m_EventBase.m_eventbase, -1, BEV_OPT_CLOSE_ON_FREE);
    if (m_IOBuffer->m_bufferevent == NULL)
    {
        g_Log.WriteError("Single base client create bufferevent failed.");
        return false;
    }

    bufferevent_setcb(m_IOBuffer->m_bufferevent, CallBack_RecvFromServer, NULL, CallBack_ClientEvent, this);
    bufferevent_enable(m_IOBuffer->m_bufferevent, EV_READ | EV_PERSIST);

    if (bufferevent_socket_connect(m_IOBuffer->m_bufferevent, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) != 0)
    {
        g_Log.WriteError("Single base client bufferevent connect failed.");
        return false;
    }

    return true;
}

void SingleEventBaseClient::CallBack_InputFromCMD(int Input, short events, void *UserData)
{
    std::vector<char> InputMessage(NETWORK_BUFFER_SIZE, 0);
    ssize_t ReadSize = read(Input, &InputMessage[0], InputMessage.size());
    if (ReadSize <= 0)
    {
        g_Log.WriteError("Single base client can not read from cmd.");
        return;
    }

    InputMessage[ReadSize - 1] = '\0';
    if (strlen(&InputMessage[0]) <= 0)
    {
        g_Log.WriteError("Single base client read empty data.");
        return;
    }

    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    Client->Send(&InputMessage[0], ReadSize);
}

void SingleEventBaseClient::CallBack_RecvUDPBroadcast(int Socket, short events, void *UserData)
{
    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    if (Client->m_IsConnected)
    {
        return;
    }

    std::vector<char> UDPBroadcastBuffer(NETWORK_BUFFER_SIZE, 0);

    socklen_t AddressLength = sizeof(socklen_t);
    ssize_t RecvSize = recvfrom(Client->m_UDPSocket, &UDPBroadcastBuffer[0], UDPBroadcastBuffer.size(), 0,
        (struct sockaddr *)&(Client->m_BroadcastAddress), &AddressLength);

    if (RecvSize == -1)
    {
        g_Log.WriteError(BoostFormat("Single base client UDP broadcast recv error = %s.", strerror(errno)));
        return;
    }

    if (RecvSize == 0)
    {
        g_Log.WriteError("Single base client UDP connection closed.");
        return;
    }

    std::string BroadcastMessage(UDPBroadcastBuffer.begin(), UDPBroadcastBuffer.end());
    g_Log.WriteDebug(BoostFormat("Single base client UDP broadcast recv message = %s.", BroadcastMessage.c_str()));
    std::string::size_type Index = BroadcastMessage.find(":");
    if (Index == std::string::npos)
    {
        g_Log.WriteError("Single base client can not parse message.");
        return;
    }

    Client->m_ServerIP = BroadcastMessage.substr(0, Index);
    Client->m_ServerPort = atoi(BroadcastMessage.substr(Index + 1, BroadcastMessage.size()).c_str());
    g_Log.WriteInfo(BoostFormat("Single base client recv connect Information: Server IP = %s, Port = %d",
        Client->m_ServerIP.c_str(), Client->m_ServerPort));

    if (!Client->ConnectServer(Client->m_ServerIP, Client->m_ServerPort))
    {
        Client->m_ServerPort = 0;
        Client->m_ServerIP.clear();
        g_Log.WriteError("Single base client Can not connect server.");
    }
}

void SingleEventBaseClient::CallBack_ClientEvent(struct bufferevent *bev, short Events, void *UserData)
{
    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    if (Events & BEV_EVENT_READING)
    {
        Client->m_IsConnected = false;
        g_Log.WriteError("Single base client error encountered while reading.");
    }
    else if (Events & BEV_EVENT_WRITING)
    {
        Client->m_IsConnected = false;
        g_Log.WriteError("Single base client error encountered while writing.");
    }
    else if (Events & BEV_EVENT_EOF)
    {
        Client->m_IsConnected = false;
        g_Log.WriteInfo("Single base client connection closed.");
    }
    else if (Events & BEV_EVENT_TIMEOUT)
    {
        g_Log.WriteError("Single base client user specified timeout reached.");
    }
    else if (Events & BEV_EVENT_CONNECTED)
    {
        Client->m_IsConnected = true;
        g_Log.WriteInfo("Single base client connected server succeed.");

        if (Client->m_UDPBroadcastEvent != NULL && Client->m_UDPBroadcastEvent->m_event != NULL)
        {
            Client->m_UDPBroadcastEvent = NULL;
            g_Log.WriteInfo("Single base client delete udp broadcast recv event and close udp socket.");
        }

        Client->ProcessConnected();
    }
    else if (Events & BEV_EVENT_ERROR)
    {
        Client->m_IsConnected = false;
        g_Log.WriteError("Single base client unrecoverable error encountered.");
    }
    else
    {
        Client->m_IsConnected = false;
        g_Log.WriteError(BoostFormat("Single base client event = %d, unknow error.", Events));
    }

    if (!Client->m_IsConnected)
    {
        if (Client->m_BroadcastPort > 0)
        {
            Client->m_ServerPort = 0;
            Client->m_ServerIP.clear();
        }

        Client->ProcessDisconnected();
    }
}

void SingleEventBaseClient::CallBack_RecvFromServer(bufferevent *bev, void *UserData)
{
    g_Log.WriteDebug("Process recv data from server call back.");
    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    Client->RecvData(0, Client->m_IOBuffer, Client->m_RecvDataBuffer);
}

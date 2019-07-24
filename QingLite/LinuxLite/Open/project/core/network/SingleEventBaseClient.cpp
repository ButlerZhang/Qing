#include "SingleEventBaseClient.h"
#include "../../../../LinuxTools.h"
#include "../tools/BoostLog.h"
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>



SingleEventBaseClient::SingleEventBaseClient()
{
    m_ServerPort = 0;
    m_UDPSocket = -1;
    m_BroadcastPort = 0;
    m_IsConnected = false;

    m_CMDInputEvent = NULL;
    m_DataBufferevent = NULL;
    m_UDPBroadcastEvent = NULL;
    m_ReBindUDPSocketTimer = NULL;
    m_ReConnectServerTimer = NULL;
    m_SendDataRandomlyTimer = NULL;
    m_EventBase = event_base_new();
}

SingleEventBaseClient::~SingleEventBaseClient()
{
    if (event_base_loopbreak(m_EventBase) != 0)
    {
        g_Log.WriteError("Client event can not loop break.");
    }

    event_free(m_CMDInputEvent);
    event_free(m_UDPBroadcastEvent);
    event_free(m_ReConnectServerTimer);
    event_free(m_ReBindUDPSocketTimer);
    event_free(m_SendDataRandomlyTimer);

    bufferevent_free(m_DataBufferevent);
    event_base_free(m_EventBase);
    g_Log.WriteDebug("Single client is release.");
}

bool SingleEventBaseClient::Start(const std::string & ServerIP, int Port)
{
    if (m_EventBase == NULL)
    {
        g_Log.WriteError("Create event base failed.");
        return false;
    }

    if (m_IsConnected)
    {
        g_Log.WriteError("Re-start single event base client.");
        return true;
    }

    if (Port <= 0 || ServerIP.empty())
    {
        g_Log.WriteError(BoostFormat("Parameters worng, port = %d, ip = %s", Port, ServerIP.c_str()));
        return false;
    }

    m_ServerPort = Port;
    m_ServerIP = ServerIP;

    if (!AddEventInputFromCMD() ||
        !AddTimerSendDataRandomly() ||
        !AddTimerReConnectServer())
    {
        return false;
    }

    g_Log.WriteInfo("Client start dispatch...");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleEventBaseClient::Start(int UDPBroadcastPort)
{
    if (m_EventBase == NULL)
    {
        g_Log.WriteError("Create event base failed.");
        return false;
    }

    if (m_IsConnected)
    {
        g_Log.WriteError("Re-start single event base client.");
        return true;
    }

    if (UDPBroadcastPort <= 0)
    {
        g_Log.WriteError(BoostFormat("Parameters worng, udp broad cast port = %d", UDPBroadcastPort));
        return false;
    }

    m_BroadcastPort = UDPBroadcastPort;

    if (!AddEventInputFromCMD() ||
        !AddTimerSendDataRandomly() ||
        !AddTimerReBindUDPSocket())
    {
        return false;
    }

    g_Log.WriteInfo("Client start dispatch...");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleEventBaseClient::Stop()
{
    if (m_EventBase == NULL)
    {
        g_Log.WriteError("Single event base client has stop.");
        return true;
    }

    if (event_base_loopbreak(m_EventBase) == 0)
    {
        g_Log.WriteDebug("Single event base client loop break.");
        event_base_free(m_EventBase);
        m_EventBase = NULL;
        return true;
    }

    g_Log.WriteError("Single event base client can not stop.");
    return false;
}

bool SingleEventBaseClient::Send(const void * Data, size_t Size)
{
    if (!m_IsConnected || m_DataBufferevent == NULL)
    {
        g_Log.WriteError("Single event base client can not send data.");
        return false;
    }

    if (bufferevent_write(m_DataBufferevent, Data, Size) != 0)
    {
        g_Log.WriteError("Single event base client send data failed.");
        return false;
    }

    g_Log.WriteInfo(BoostFormat("Single event base client send succeed, size = %d.", Size));
    return true;
}

bool SingleEventBaseClient::ConnectServer(const std::string &ServerIP, int Port)
{
    if (m_IsConnected)
    {
        g_Log.WriteError("Re-connect server.");
        return true;
    }

    struct sockaddr_in ServerAddress;
    bzero(&ServerAddress, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP.c_str(), &(ServerAddress.sin_addr));
    ServerAddress.sin_port = htons(static_cast<uint16_t>(Port));

    if (m_DataBufferevent == NULL)
    {
        m_DataBufferevent = bufferevent_socket_new(m_EventBase, -1, BEV_OPT_CLOSE_ON_FREE);
        if (m_DataBufferevent == NULL)
        {
            g_Log.WriteError("Create bufferevent failed.");
            return false;
        }

        bufferevent_setcb(m_DataBufferevent, CallBack_RecvFromServer, NULL, CallBack_ClientEvent, this);
        bufferevent_enable(m_DataBufferevent, EV_READ | EV_PERSIST);
    }

    int ConnectResult = bufferevent_socket_connect(m_DataBufferevent, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
    if (ConnectResult != 0)
    {
        g_Log.WriteError("bufferevent connect failed.");
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddEventInputFromCMD()
{
    if (m_CMDInputEvent != NULL)
    {
        g_Log.WriteError("Re-create CMD event.");
        return true;
    }

    m_CMDInputEvent = event_new(m_EventBase, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_InputFromCMD, this);
    if (m_CMDInputEvent == NULL)
    {
        g_Log.WriteError("Create CMD event failed.");
        return false;
    }

    if (event_add(m_CMDInputEvent, NULL) != 0)
    {
        g_Log.WriteError("Add CMD event failed.");
        event_free(m_CMDInputEvent);
        m_CMDInputEvent = NULL;
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddEventRecvUDPBroadcast()
{
    m_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_UDPSocket == -1)
    {
        g_Log.WriteError("Create udp socket failed.");
        return false;
    }

    int Optval = 1;
    if (setsockopt(m_UDPSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &Optval, sizeof(int)) < 0)
    {
        g_Log.WriteError("Set udp sockopt failed.");
        return false;
    }

    bzero(&m_BroadcastAddress, sizeof(sockaddr_in));
    m_BroadcastAddress.sin_family = AF_INET;
    m_BroadcastAddress.sin_addr.s_addr = INADDR_ANY;
    m_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(m_BroadcastPort));

    if (bind(m_UDPSocket, (struct sockaddr *)&m_BroadcastAddress, sizeof(m_BroadcastAddress)) == -1)
    {
        g_Log.WriteError("Bind udp socket failed.");
        return false;
    }

    m_UDPBroadcastEvent = event_new(m_EventBase, m_UDPSocket, EV_READ | EV_PERSIST, CallBack_RecvUDPBroadcast, this);
    if (m_UDPBroadcastEvent == NULL)
    {
        g_Log.WriteError("Create udp broadcast event failed.");
        return false;
    }

    if (event_add(m_UDPBroadcastEvent, NULL) == -1)
    {
        g_Log.WriteError("Add udp broadcast event failed.");
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddTimerReBindUDPSocket()
{
    if (m_ReBindUDPSocketTimer != NULL)
    {
        g_Log.WriteError("Re-create re-bind udp socket timer.");
        return true;
    }

    m_ReBindUDPSocketTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_ReBindUDPSocket, this);
    if (m_ReBindUDPSocketTimer == NULL)
    {
        g_Log.WriteError("Create re-bind udp socket timer failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 1;

    if (event_add(m_ReBindUDPSocketTimer, &tv) != 0)
    {
        g_Log.WriteError("Add re-bind udp socket timer failed.");
        event_free(m_ReBindUDPSocketTimer);
        m_ReBindUDPSocketTimer = NULL;
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddTimerReConnectServer()
{
    if (m_ReConnectServerTimer != NULL)
    {
        g_Log.WriteError("Re-create re-connect server timer.");
        return true;
    }

    m_ReConnectServerTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_ReConnectServer, this);
    if (m_ReConnectServerTimer == NULL)
    {
        g_Log.WriteError("Create re-connect server timer failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 1;

    if (event_add(m_ReConnectServerTimer, &tv) != 0)
    {
        g_Log.WriteError("Add re-connect server timer failed.");
        event_free(m_ReConnectServerTimer);
        m_ReConnectServerTimer = NULL;
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddTimerSendDataRandomly()
{
    return true;
    if (m_SendDataRandomlyTimer != NULL)
    {
        g_Log.WriteError("Re-create send data randomly event.");
        return true;
    }

    m_SendDataRandomlyTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_SendDataRandomly, this);
    if (m_SendDataRandomlyTimer == NULL)
    {
        g_Log.WriteError("Create send data randomly event failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = GetRandomUIntInRange(3, 10);

    if (event_add(m_SendDataRandomlyTimer, &tv) != 0)
    {
        g_Log.WriteError("Add send data randomly event failed.");
        event_free(m_SendDataRandomlyTimer);
        m_SendDataRandomlyTimer = NULL;
        return false;
    }

    return true;
}

void SingleEventBaseClient::CallBack_InputFromCMD(int Input, short events, void *UserData)
{
    char InputMessage[NETWORK_BUFFER_SIZE];
    memset(InputMessage, 0, sizeof(InputMessage));

    ssize_t ReadSize = read(Input, InputMessage, sizeof(InputMessage));
    if (ReadSize <= 0)
    {
        g_Log.WriteError("Can not read from cmd.");
        return;
    }

    InputMessage[ReadSize - 1] = '\0';
    if (strlen(InputMessage) <= 0)
    {
        g_Log.WriteError("Read empty data.");
        return;
    }

    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    if (Client->m_DataBufferevent == NULL || !Client->m_IsConnected)
    {
        g_Log.WriteError("Can not send data, not connect server.");
        return;
    }

    if (bufferevent_write(Client->m_DataBufferevent, InputMessage, ReadSize) == 0)
    {
        g_Log.WriteDebug(BoostFormat("Send message = %s, size = %d.", InputMessage, ReadSize));
    }
    else
    {
        g_Log.WriteError(BoostFormat("Send message = %s failed.\n", InputMessage));
    }
}

void SingleEventBaseClient::CallBack_RecvUDPBroadcast(int Socket, short events, void *UserData)
{
    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    if (Client->m_IsConnected)
    {
        return;
    }

    char UDPBroadcastBuffer[NETWORK_BUFFER_SIZE];
    memset(UDPBroadcastBuffer, 0, sizeof(UDPBroadcastBuffer));

    socklen_t AddressLength = sizeof(socklen_t);
    ssize_t RecvSize = recvfrom(Client->m_UDPSocket,
        UDPBroadcastBuffer, sizeof(UDPBroadcastBuffer), 0,
        (struct sockaddr *)&(Client->m_BroadcastAddress), &AddressLength);

    if (RecvSize == -1)
    {
        g_Log.WriteError("UDP broadcast recv error.");
        return;
    }

    if (RecvSize == 0)
    {
        g_Log.WriteError("UDP connection closed.");
        return;
    }

    std::string BroadcastMessage(UDPBroadcastBuffer);
    g_Log.WriteDebug(BoostFormat("UDP broadcast recv message = %s.", UDPBroadcastBuffer));
    std::string::size_type Index = BroadcastMessage.find(":");
    if (Index == std::string::npos)
    {
        g_Log.WriteError("Can not parse message.");
        return;
    }

    Client->m_ServerIP = BroadcastMessage.substr(0, Index);
    Client->m_ServerPort = atoi(BroadcastMessage.substr(Index + 1, BroadcastMessage.size()).c_str());
    g_Log.WriteInfo(BoostFormat("Connect Information: Server IP = %s, Port = %d", Client->m_ServerIP.c_str(), Client->m_ServerPort));

    if (!Client->ConnectServer(Client->m_ServerIP, Client->m_ServerPort))
    {
        Client->m_ServerPort = 0;
        Client->m_ServerIP.clear();
        g_Log.WriteError("Can not connect server.");
    }
}

void SingleEventBaseClient::CallBack_ReBindUDPSocket(int Socket, short Events, void * UserData)
{
    int TimerInternal = 5;
    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;

    if (Client->m_UDPSocket > 0)
    {
        close(Client->m_UDPSocket);
        Client->m_UDPSocket = -1;
    }

    if (Client->m_UDPBroadcastEvent != NULL)
    {
        event_free(Client->m_UDPBroadcastEvent);
        Client->m_UDPBroadcastEvent = NULL;
    }

    if (!Client->m_IsConnected)
    {
        if (Client->AddEventRecvUDPBroadcast() == false)
        {
            TimerInternal = 1;
        }
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = TimerInternal;
    event_add(Client->m_ReBindUDPSocketTimer, &tv);
}

void SingleEventBaseClient::CallBack_ReConnectServer(int Socket, short Events, void * UserData)
{
    int TimerInternal = 5;
    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    if (!Client->m_IsConnected && Client->m_DataBufferevent == NULL)
    {
        if (Client->ConnectServer(Client->m_ServerIP, Client->m_ServerPort) == false)
        {
            TimerInternal = 1;
        }
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = TimerInternal;
    event_add(Client->m_ReConnectServerTimer, &tv);
}

void SingleEventBaseClient::CallBack_SendDataRandomly(int Socket, short Events, void *UserData)
{
    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    if (Client->m_IsConnected && Client->m_DataBufferevent != NULL)
    {
        const std::string &UUID = GetUUID();
        Client->Send(UUID.c_str(), UUID.size());

        struct timeval tv;
        evutil_timerclear(&tv);
        tv.tv_sec = GetRandomUIntInRange(3, 10);
        event_add(Client->m_SendDataRandomlyTimer, &tv);
    }
}

void SingleEventBaseClient::CallBack_ClientEvent(struct bufferevent *bev, short Events, void *UserData)
{
    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    if (Events & BEV_EVENT_READING)
    {
        Client->m_IsConnected = false;
        g_Log.WriteError("error encountered while reading");
    }
    else if (Events & BEV_EVENT_WRITING)
    {
        Client->m_IsConnected = false;
        g_Log.WriteError("error encountered while writing");
    }
    else if (Events & BEV_EVENT_EOF)
    {
        Client->m_IsConnected = false;
        int ClientSocket = bufferevent_getfd(bev);
        g_Log.WriteInfo(BoostFormat("Client = %d connection closed.", ClientSocket));
    }
    else if (Events & BEV_EVENT_TIMEOUT)
    {
        g_Log.WriteError("User specified timeout reached.");
    }
    else if (Events & BEV_EVENT_CONNECTED)
    {
        Client->m_IsConnected = true;
        g_Log.WriteInfo("Connected server succeed.");
        if (Client->m_UDPBroadcastEvent != NULL)
        {
            if (event_del(Client->m_UDPBroadcastEvent) == 0)
            {
                g_Log.WriteInfo("Delete udp broadcast recv event and close udp socket.");
                event_free(Client->m_UDPBroadcastEvent);
                Client->m_UDPBroadcastEvent = NULL;
                close(Client->m_UDPSocket);
            }
        }

        Client->ProcessConnected();
    }
    else if(Events & BEV_EVENT_ERROR)
    {
        Client->m_IsConnected = false;
        g_Log.WriteError("unrecoverable error encountered");
    }
    else
    {
        Client->m_IsConnected = false;
        g_Log.WriteError(BoostFormat("Client event = %d, unknow error.", Events));
    }

    if (!Client->m_IsConnected)
    {
        if (Client->m_DataBufferevent != NULL)
        {
            bufferevent_free(Client->m_DataBufferevent);
            Client->m_DataBufferevent = NULL;
        }

        Client->ProcessDisconnected();
    }
}

void SingleEventBaseClient::CallBack_RecvFromServer(bufferevent *bev, void *UserData)
{
    char ServerMessage[NETWORK_BUFFER_SIZE];
    memset(ServerMessage, 0, sizeof(ServerMessage));

    size_t RecvSize = bufferevent_read(bev, ServerMessage, sizeof(ServerMessage));
    g_Log.WriteInfo(BoostFormat("Recv message size = %d", RecvSize));

    if (RecvSize > 0)
    {
        NetworkMessage NetworkMsg;
        NetworkMsg.m_Message.assign(ServerMessage, ServerMessage + RecvSize);

        SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
        Client->ProcessMessage(NetworkMsg);
    }
}

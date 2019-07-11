#include "SingleEventBaseClient.h"
#include "../../../LinuxTools.h"
#include "../Tools/BoostLog.h"
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
    event_free(m_CMDInputEvent);
    event_free(m_UDPBroadcastEvent);
    event_free(m_ReConnectServerTimer);
    event_free(m_ReBindUDPSocketTimer);
    event_free(m_SendDataRandomlyTimer);

    bufferevent_free(m_DataBufferevent);
    event_base_free(m_EventBase);
}

bool SingleEventBaseClient::Start(const std::string & ServerIP, int Port)
{
    if (m_EventBase == NULL)
    {
        BoostLog::WriteError("Create event base failed.");
        return false;
    }

    if (m_IsConnected)
    {
        BoostLog::WriteError("Re-start single event base client.");
        return true;
    }

    if (Port <= 0 || ServerIP.empty())
    {
        BoostLog::WriteError(BoostFormat("Parameters worng, port = %d, ip = %s", Port, ServerIP.c_str()));
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

    BoostLog::WriteInfo("Client start dispatch...");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleEventBaseClient::Start(int UDPBroadcastPort)
{
    if (m_EventBase == NULL)
    {
        BoostLog::WriteError("Create event base failed.");
        return false;
    }

    if (m_IsConnected)
    {
        BoostLog::WriteError("Re-start single event base client.");
        return true;
    }

    if (UDPBroadcastPort <= 0)
    {
        BoostLog::WriteError(BoostFormat("Parameters worng, udp broad cast port = %d", UDPBroadcastPort));
        return false;
    }

    m_BroadcastPort = UDPBroadcastPort;

    if (!AddEventInputFromCMD() ||
        !AddTimerSendDataRandomly() ||
        !AddTimerReBindUDPSocket())
    {
        return false;
    }

    BoostLog::WriteInfo("Client start dispatch...");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleEventBaseClient::Stop()
{
    if (m_EventBase == NULL)
    {
        BoostLog::WriteError("Single event base client has stop.");
        return true;
    }

    if (event_base_loopbreak(m_EventBase) == 0)
    {
        BoostLog::WriteDebug("Single event base client loop break.");
        event_base_free(m_EventBase);
        m_EventBase = NULL;
        return true;
    }

    BoostLog::WriteError("Single event base client can not stop.");
    return false;
}

bool SingleEventBaseClient::Send(const void * Data, size_t Size)
{
    if (!m_IsConnected || m_DataBufferevent == NULL)
    {
        BoostLog::WriteError("Single event base client can not send data.");
        return false;
    }

    if (bufferevent_write(m_DataBufferevent, Data, Size) != 0)
    {
        BoostLog::WriteError("Single event base client send data failed.");
        return false;
    }

    BoostLog::WriteInfo(BoostFormat("Single event base client send succeed, size = %d.", Size));
    return true;
}

bool SingleEventBaseClient::ConnectServer(const std::string &ServerIP, int Port)
{
    if (m_IsConnected)
    {
        BoostLog::WriteError("Re-connect server.");
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
            BoostLog::WriteError("Create bufferevent failed.");
            return false;
        }

        bufferevent_setcb(m_DataBufferevent, CallBack_RecvFromServer, NULL, CallBack_ClientEvent, this);
        bufferevent_enable(m_DataBufferevent, EV_READ | EV_PERSIST);
    }

    int ConnectResult = bufferevent_socket_connect(m_DataBufferevent, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
    if (ConnectResult != 0)
    {
        BoostLog::WriteError("bufferevent connect failed.");
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddEventInputFromCMD()
{
    if (m_CMDInputEvent != NULL)
    {
        BoostLog::WriteError("Re-create CMD event.");
        return true;
    }

    m_CMDInputEvent = event_new(m_EventBase, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_InputFromCMD, this);
    if (m_CMDInputEvent == NULL)
    {
        BoostLog::WriteError("Create CMD event failed.");
        return false;
    }

    if (event_add(m_CMDInputEvent, NULL) != 0)
    {
        BoostLog::WriteError("Add CMD event failed.");
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
        BoostLog::WriteError("Create udp socket failed.");
        return false;
    }

    int Optval = 1;
    if (setsockopt(m_UDPSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &Optval, sizeof(int)) < 0)
    {
        BoostLog::WriteError("Set udp sockopt failed.");
        return false;
    }

    bzero(&m_BroadcastAddress, sizeof(sockaddr_in));
    m_BroadcastAddress.sin_family = AF_INET;
    m_BroadcastAddress.sin_addr.s_addr = INADDR_ANY;
    m_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(m_BroadcastPort));

    if (bind(m_UDPSocket, (struct sockaddr *)&m_BroadcastAddress, sizeof(m_BroadcastAddress)) == -1)
    {
        BoostLog::WriteError("Bind udp socket failed.");
        return false;
    }

    m_UDPBroadcastEvent = event_new(m_EventBase, m_UDPSocket, EV_READ | EV_PERSIST, CallBack_RecvUDPBroadcast, this);
    if (m_UDPBroadcastEvent == NULL)
    {
        BoostLog::WriteError("Create udp broadcast event failed.");
        return false;
    }

    if (event_add(m_UDPBroadcastEvent, NULL) == -1)
    {
        BoostLog::WriteError("Add udp broadcast event failed.");
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddTimerReBindUDPSocket()
{
    if (m_ReBindUDPSocketTimer != NULL)
    {
        BoostLog::WriteError("Re-create re-bind udp socket timer.");
        return true;
    }

    m_ReBindUDPSocketTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_ReBindUDPSocket, this);
    if (m_ReBindUDPSocketTimer == NULL)
    {
        BoostLog::WriteError("Create re-bind udp socket timer failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 1;

    if (event_add(m_ReBindUDPSocketTimer, &tv) != 0)
    {
        BoostLog::WriteError("Add re-bind udp socket timer failed.");
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
        BoostLog::WriteError("Re-create re-connect server timer.");
        return true;
    }

    m_ReConnectServerTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_ReConnectServer, this);
    if (m_ReConnectServerTimer == NULL)
    {
        BoostLog::WriteError("Create re-connect server timer failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 1;

    if (event_add(m_ReConnectServerTimer, &tv) != 0)
    {
        BoostLog::WriteError("Add re-connect server timer failed.");
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
        BoostLog::WriteError("Re-create send data randomly event.");
        return true;
    }

    m_SendDataRandomlyTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_SendDataRandomly, this);
    if (m_SendDataRandomlyTimer == NULL)
    {
        BoostLog::WriteError("Create send data randomly event failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = GetRandomUIntInRange(3, 10);

    if (event_add(m_SendDataRandomlyTimer, &tv) != 0)
    {
        BoostLog::WriteError("Add send data randomly event failed.");
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
        BoostLog::WriteError("Can not read from cmd.");
        return;
    }

    InputMessage[ReadSize - 1] = '\0';
    if (strlen(InputMessage) <= 0)
    {
        BoostLog::WriteError("Read empty data.");
        return;
    }

    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    if (Client->m_DataBufferevent == NULL || !Client->m_IsConnected)
    {
        BoostLog::WriteError("Can not send data, not connect server.");
        return;
    }

    if (bufferevent_write(Client->m_DataBufferevent, InputMessage, ReadSize) == 0)
    {
        BoostLog::WriteDebug(BoostFormat("Send message = %s, size = %d.", InputMessage, ReadSize));
    }
    else
    {
        BoostLog::WriteError(BoostFormat("Send message = %s failed.\n", InputMessage));
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
        BoostLog::WriteError("UDP broadcast recv error.");
        return;
    }

    if (RecvSize == 0)
    {
        BoostLog::WriteError("UDP connection closed.");
        return;
    }

    std::string BroadcastMessage(UDPBroadcastBuffer);
    BoostLog::WriteDebug(BoostFormat("UDP broadcast recv message = %s.", UDPBroadcastBuffer));
    std::string::size_type Index = BroadcastMessage.find(":");
    if (Index == std::string::npos)
    {
        BoostLog::WriteError("Can not parse message.");
        return;
    }

    Client->m_ServerIP = BroadcastMessage.substr(0, Index);
    Client->m_ServerPort = atoi(BroadcastMessage.substr(Index + 1, BroadcastMessage.size()).c_str());
    BoostLog::WriteInfo(BoostFormat("Connect Information: Server IP = %s, Port = %d", Client->m_ServerIP.c_str(), Client->m_ServerPort));

    if (!Client->ConnectServer(Client->m_ServerIP, Client->m_ServerPort))
    {
        Client->m_ServerPort = 0;
        Client->m_ServerIP.clear();
        BoostLog::WriteError("Can not connect server.");
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
        BoostLog::WriteError("error encountered while reading");
    }
    else if (Events & BEV_EVENT_WRITING)
    {
        Client->m_IsConnected = false;
        BoostLog::WriteError("error encountered while writing");
    }
    else if (Events & BEV_EVENT_EOF)
    {
        Client->m_IsConnected = false;
        int ClientSocket = bufferevent_getfd(bev);
        BoostLog::WriteInfo(BoostFormat("Client = %d connection closed.", ClientSocket));
    }
    else if (Events & BEV_EVENT_TIMEOUT)
    {
        BoostLog::WriteError("User specified timeout reached.");
    }
    else if (Events & BEV_EVENT_CONNECTED)
    {
        Client->m_IsConnected = true;
        BoostLog::WriteInfo("Connected server succeed.");
        if (Client->m_UDPBroadcastEvent != NULL)
        {
            if (event_del(Client->m_UDPBroadcastEvent) == 0)
            {
                BoostLog::WriteInfo("Delete udp broadcast recv event and close udp socket.");
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
        BoostLog::WriteError("unrecoverable error encountered");
    }
    else
    {
        Client->m_IsConnected = false;
        BoostLog::WriteError(BoostFormat("Client event = %d, unknow error.", Events));
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
    BoostLog::WriteInfo(BoostFormat("Recv message size = %d", RecvSize));

    if (RecvSize > 0)
    {
        NetworkMessage NetworkMsg;
        NetworkMsg.m_Message.assign(ServerMessage, ServerMessage + RecvSize);

        SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
        Client->ProcessMessage(NetworkMsg);
    }
}

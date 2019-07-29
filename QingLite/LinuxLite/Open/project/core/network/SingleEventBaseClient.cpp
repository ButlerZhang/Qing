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

    m_RecvBuffer = evbuffer_new();
    m_EventBase = event_base_new();
}

SingleEventBaseClient::~SingleEventBaseClient()
{
    if (event_base_loopbreak(m_EventBase) != 0)
    {
        g_Log.WriteError("Single base client event can not loop break.");
    }

    if (m_CMDInputEvent != NULL)
    {
        event_free(m_CMDInputEvent);
        m_CMDInputEvent = NULL;
    }

    if (m_UDPBroadcastEvent != NULL)
    {
        event_free(m_UDPBroadcastEvent);
        m_UDPBroadcastEvent = NULL;
    }

    if (m_ReConnectServerTimer != NULL)
    {
        event_free(m_ReConnectServerTimer);
    }

    if (m_ReBindUDPSocketTimer != NULL)
    {
        event_free(m_ReBindUDPSocketTimer);
    }

    if (m_SendDataRandomlyTimer != NULL)
    {
        event_free(m_SendDataRandomlyTimer);
    }

    if (m_DataBufferevent != NULL)
    {
        bufferevent_free(m_DataBufferevent);
    }

    evbuffer_free(m_RecvBuffer);
    event_base_free(m_EventBase);
    g_Log.WriteDebug("Single base client was destructored.");
}

bool SingleEventBaseClient::Start(const std::string & ServerIP, int Port)
{
    if (m_EventBase == NULL)
    {
        g_Log.WriteError("Single base client create event base failed.");
        return false;
    }

    if (m_IsConnected)
    {
        g_Log.WriteError("Single base client re-start.");
        return true;
    }

    if (Port <= 0 || ServerIP.empty())
    {
        g_Log.WriteError(BoostFormat("Single base client parameters worng, port = %d, ip = %s", Port, ServerIP.c_str()));
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

    g_Log.WriteInfo("Single base client start dispatch...");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleEventBaseClient::Start(int UDPBroadcastPort)
{
    if (m_EventBase == NULL)
    {
        g_Log.WriteError("Single base client create event base failed.");
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

    if (!AddEventInputFromCMD() ||
        !AddTimerSendDataRandomly() ||
        !AddTimerReBindUDPSocket())
    {
        return false;
    }

    g_Log.WriteInfo("Single base client start dispatch...");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleEventBaseClient::Send(const void * Data, size_t Size)
{
    if (!m_IsConnected || m_DataBufferevent == NULL)
    {
        g_Log.WriteError("Single base client can not send data, no connected.");
        return false;
    }

    if (bufferevent_write(m_DataBufferevent, Data, Size) != 0)
    {
        g_Log.WriteError("Single base client send data failed.");
        return false;
    }

    g_Log.WriteDebug(BoostFormat("Single base client send succeed, size = %d.", Size));
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
    bzero(&ServerAddress, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP.c_str(), &(ServerAddress.sin_addr));
    ServerAddress.sin_port = htons(static_cast<uint16_t>(Port));

    if (m_DataBufferevent == NULL)
    {
        m_DataBufferevent = bufferevent_socket_new(m_EventBase, -1, BEV_OPT_CLOSE_ON_FREE);
        if (m_DataBufferevent == NULL)
        {
            g_Log.WriteError("Single base client create bufferevent failed.");
            return false;
        }

        bufferevent_setcb(m_DataBufferevent, CallBack_RecvFromServer, NULL, CallBack_ClientEvent, this);
        bufferevent_enable(m_DataBufferevent, EV_READ | EV_PERSIST);
    }

    int ConnectResult = bufferevent_socket_connect(m_DataBufferevent, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
    if (ConnectResult != 0)
    {
        g_Log.WriteError("Single base client bufferevent connect failed.");
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddEventInputFromCMD()
{
    if (m_CMDInputEvent != NULL)
    {
        g_Log.WriteError("Single base client re-create CMD event.");
        return true;
    }

    m_CMDInputEvent = event_new(m_EventBase, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_InputFromCMD, this);
    if (m_CMDInputEvent == NULL)
    {
        g_Log.WriteError("Single base client create CMD event failed.");
        return false;
    }

    if (event_add(m_CMDInputEvent, NULL) != 0)
    {
        g_Log.WriteError("Single base client add CMD event failed.");
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
        g_Log.WriteError("Single base client create udp socket failed.");
        return false;
    }

    int Optval = 1;
    if (setsockopt(m_UDPSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &Optval, sizeof(int)) < 0)
    {
        g_Log.WriteError("Single base client set udp sockopt failed.");
        return false;
    }

    bzero(&m_BroadcastAddress, sizeof(sockaddr_in));
    m_BroadcastAddress.sin_family = AF_INET;
    m_BroadcastAddress.sin_addr.s_addr = INADDR_ANY;
    m_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(m_BroadcastPort));

    if (bind(m_UDPSocket, (struct sockaddr *)&m_BroadcastAddress, sizeof(m_BroadcastAddress)) == -1)
    {
        g_Log.WriteError("Single base client bind udp socket failed.");
        return false;
    }

    m_UDPBroadcastEvent = event_new(m_EventBase, m_UDPSocket, EV_READ | EV_PERSIST, CallBack_RecvUDPBroadcast, this);
    if (m_UDPBroadcastEvent == NULL)
    {
        g_Log.WriteError("Single base client create udp broadcast event failed.");
        return false;
    }

    if (event_add(m_UDPBroadcastEvent, NULL) == -1)
    {
        g_Log.WriteError("Single base client add udp broadcast event failed.");
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddTimerReBindUDPSocket()
{
    if (m_ReBindUDPSocketTimer != NULL)
    {
        g_Log.WriteError("Single base client re-create re-bind udp socket timer.");
        return true;
    }

    m_ReBindUDPSocketTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_ReBindUDPSocket, this);
    if (m_ReBindUDPSocketTimer == NULL)
    {
        g_Log.WriteError("Single base client create re-bind udp socket timer failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 1;

    if (event_add(m_ReBindUDPSocketTimer, &tv) != 0)
    {
        g_Log.WriteError("Single base client add re-bind udp socket timer failed.");
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
        g_Log.WriteError("Single base client re-create re-connect server timer.");
        return true;
    }

    m_ReConnectServerTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_ReConnectServer, this);
    if (m_ReConnectServerTimer == NULL)
    {
        g_Log.WriteError("Single base client create re-connect server timer failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 1;

    if (event_add(m_ReConnectServerTimer, &tv) != 0)
    {
        g_Log.WriteError("Single base client add re-connect server timer failed.");
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
        g_Log.WriteError("Single base client re-create send data randomly event.");
        return true;
    }

    m_SendDataRandomlyTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_SendDataRandomly, this);
    if (m_SendDataRandomlyTimer == NULL)
    {
        g_Log.WriteError("Single base client create send data randomly event failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = GetRandomUIntInRange(3, 10);

    if (event_add(m_SendDataRandomlyTimer, &tv) != 0)
    {
        g_Log.WriteError("Single base client add send data randomly event failed.");
        event_free(m_SendDataRandomlyTimer);
        m_SendDataRandomlyTimer = NULL;
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
    if (Client->m_DataBufferevent == NULL || !Client->m_IsConnected)
    {
        g_Log.WriteError("Single base client can not send data, not connect server.");
        return;
    }

    if (bufferevent_write(Client->m_DataBufferevent, &InputMessage[0], ReadSize) == 0)
    {
        g_Log.WriteDebug(BoostFormat("Single base client send message = %s, size = %d.", &InputMessage[0], ReadSize));
    }
    else
    {
        g_Log.WriteError(BoostFormat("Single base client send message = %s failed.\n", &InputMessage[0]));
    }
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
    ssize_t RecvSize = recvfrom(Client->m_UDPSocket,
        &UDPBroadcastBuffer[0], UDPBroadcastBuffer.size(), 0,
        (struct sockaddr *)&(Client->m_BroadcastAddress), &AddressLength);

    if (RecvSize == -1)
    {
        g_Log.WriteError("Single base client UDP broadcast recv error.");
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
    g_Log.WriteInfo(BoostFormat("Single base client connect Information: Server IP = %s, Port = %d", Client->m_ServerIP.c_str(), Client->m_ServerPort));

    if (!Client->ConnectServer(Client->m_ServerIP, Client->m_ServerPort))
    {
        Client->m_ServerPort = 0;
        Client->m_ServerIP.clear();
        g_Log.WriteError("Single base client Can not connect server.");
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
        g_Log.WriteError("Single base client error encountered while reading");
    }
    else if (Events & BEV_EVENT_WRITING)
    {
        Client->m_IsConnected = false;
        g_Log.WriteError("Single base client error encountered while writing");
    }
    else if (Events & BEV_EVENT_EOF)
    {
        Client->m_IsConnected = false;
        int ClientSocket = bufferevent_getfd(bev);
        g_Log.WriteInfo(BoostFormat("Single base client = %d connection closed.", ClientSocket));
    }
    else if (Events & BEV_EVENT_TIMEOUT)
    {
        g_Log.WriteError("Single base client user specified timeout reached.");
    }
    else if (Events & BEV_EVENT_CONNECTED)
    {
        Client->m_IsConnected = true;
        g_Log.WriteInfo("Single base client connected server succeed.");
        if (Client->m_UDPBroadcastEvent != NULL)
        {
            if (event_del(Client->m_UDPBroadcastEvent) == 0)
            {
                g_Log.WriteInfo("Single base client delete udp broadcast recv event and close udp socket.");
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
        g_Log.WriteError("Single base client unrecoverable error encountered");
    }
    else
    {
        Client->m_IsConnected = false;
        g_Log.WriteError(BoostFormat("Single base client event = %d, unknow error.", Events));
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
    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    g_Log.WriteDebug("Process recv from server call back.");

    const size_t MESSAGE_HEADER_LENGTH_SIZE = 4;
    std::vector<char> MessageHeaderLengthBuffer(MESSAGE_HEADER_LENGTH_SIZE, 0);

    std::vector<char> RecvBuffer(NETWORK_BUFFER_SIZE, 0);
    struct evbuffer *EventBuffer = Client->m_RecvBuffer;

    size_t RecvSize = 0, EventBufferLength = 0;
    while (RecvSize = bufferevent_read(bev, &RecvBuffer[0], RecvBuffer.size()), RecvSize > 0)
    {
        g_Log.WriteDebug(BoostFormat("Recv message size = %llu", RecvSize));

        if (evbuffer_add(EventBuffer, &RecvBuffer[0], RecvSize) != 0)
        {
            g_Log.WriteError("Recv message can not add to evbuffer.");
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
            NetworkMsg.m_Message.assign(RecvBuffer.begin(), RecvBuffer.begin() + MessageTotalLength);
            Client->ProcessMessage(NetworkMsg);
        }
    }
}

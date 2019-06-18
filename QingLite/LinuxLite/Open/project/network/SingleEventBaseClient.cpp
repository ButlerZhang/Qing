#include "SingleEventBaseClient.h"
#include "../../../LinuxTools.h"
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
        printf("ERROR: Create event base failed.\n");
        return false;
    }

    if (m_IsConnected)
    {
        return true;
    }

    if (Port <= 0 || ServerIP.empty())
    {
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

    printf("Client start dispatch...\n");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleEventBaseClient::Start(int UDPBroadcastPort)
{
    if (m_EventBase == NULL)
    {
        printf("ERROR: Create event base failed.\n");
        return false;
    }

    if (m_IsConnected)
    {
        return true;
    }

    if (UDPBroadcastPort <= 0)
    {
        return false;
    }

    m_BroadcastPort = UDPBroadcastPort;

    if (!AddEventInputFromCMD() ||
        !AddTimerSendDataRandomly() ||
        !AddTimerReBindUDPSocket())
    {
        return false;
    }

    printf("Client start dispatch...\n");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleEventBaseClient::Stop()
{
    if (m_EventBase == NULL)
    {
        return true;
    }

    if (event_base_loopbreak(m_EventBase) == 0)
    {
        event_base_free(m_EventBase);
        m_EventBase = NULL;
        return true;
    }

    return false;
}

bool SingleEventBaseClient::Send(const void * Data, size_t Size)
{
    if (!m_IsConnected || m_DataBufferevent == NULL)
    {
        return false;
    }

    if (bufferevent_write(m_DataBufferevent, Data, Size) != 0)
    {
        printf("Send failed.\n");
        return false;
    }

    printf("Send succeed.\n");
    return true;
}

bool SingleEventBaseClient::ConnectServer(const std::string &ServerIP, int Port)
{
    if (m_IsConnected)
    {
        printf("ERROR: Re-connect server.\n");
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
            printf("ERROR: Create bufferevent failed.\n");
            return false;
        }

        bufferevent_setcb(m_DataBufferevent, CallBack_RecvFromServer, NULL, CallBack_ClientEvent, this);
        bufferevent_enable(m_DataBufferevent, EV_READ | EV_PERSIST);
    }

    int ConnectResult = bufferevent_socket_connect(m_DataBufferevent, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
    if (ConnectResult != 0)
    {
        printf("ERROR: bufferevent connect failed.\n");
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddEventInputFromCMD()
{
    if (m_CMDInputEvent != NULL)
    {
        printf("ERROR: Re-create CMD event.\n");
        return true;
    }

    m_CMDInputEvent = event_new(m_EventBase, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_InputFromCMD, this);
    if (m_CMDInputEvent == NULL)
    {
        printf("ERROR: Create CMD event failed.\n");
        return false;
    }

    if (event_add(m_CMDInputEvent, NULL) != 0)
    {
        printf("ERROR: Add CMD event failed.\n");
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
        printf("ERROR: Create udp socket failed.\n");
        return false;
    }

    int Optval = 1;
    if (setsockopt(m_UDPSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &Optval, sizeof(int)) < 0)
    {
        printf("ERROR: set udp sockopt failed.\n");
        return false;
    }

    bzero(&m_BroadcastAddress, sizeof(sockaddr_in));
    m_BroadcastAddress.sin_family = AF_INET;
    m_BroadcastAddress.sin_addr.s_addr = INADDR_ANY;
    m_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(m_BroadcastPort));

    if (bind(m_UDPSocket, (struct sockaddr *)&m_BroadcastAddress, sizeof(m_BroadcastAddress)) == -1)
    {
        printf("ERROR: Bind udp socket failed.\n");
        return false;
    }

    m_UDPBroadcastEvent = event_new(m_EventBase, m_UDPSocket, EV_READ | EV_PERSIST, CallBack_RecvUDPBroadcast, this);
    if (m_UDPBroadcastEvent == NULL)
    {
        printf("ERROR: Create udp broadcast event failed.\n");
        return false;
    }

    if (event_add(m_UDPBroadcastEvent, NULL) == -1)
    {
        printf("ERROR: Add udp broadcast event failed.\n");
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddTimerReBindUDPSocket()
{
    if (m_ReBindUDPSocketTimer != NULL)
    {
        printf("ERROR: Re-create re-bind udp socket timer.\n");
        return true;
    }

    m_ReBindUDPSocketTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_ReBindUDPSocket, this);
    if (m_ReBindUDPSocketTimer == NULL)
    {
        printf("ERROR: Create re-bind udp socket timer failed.\n");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 1;

    if (event_add(m_ReBindUDPSocketTimer, &tv) != 0)
    {
        printf("ERROR: Add re-bind udp socket timer failed.\n");
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
        printf("ERROR: Re-create re-connect server timer.\n");
        return true;
    }

    m_ReConnectServerTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_ReConnectServer, this);
    if (m_ReConnectServerTimer == NULL)
    {
        printf("ERROR: Create re-connect server timer failed.\n");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 1;

    if (event_add(m_ReConnectServerTimer, &tv) != 0)
    {
        printf("ERROR: Add re-connect server timer failed.\n");
        event_free(m_ReConnectServerTimer);
        m_ReConnectServerTimer = NULL;
        return false;
    }

    return true;
}

bool SingleEventBaseClient::AddTimerSendDataRandomly()
{
    if (m_SendDataRandomlyTimer != NULL)
    {
        printf("ERROR: Re-create send data randomly event.\n");
        return true;
    }

    m_SendDataRandomlyTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_SendDataRandomly, this);
    if (m_SendDataRandomlyTimer == NULL)
    {
        printf("ERROR: Create send data randomly event failed.\n");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = GetRandomUIntInRange(3, 10);

    if (event_add(m_SendDataRandomlyTimer, &tv) != 0)
    {
        printf("ERROR: Add send data randomly event failed.\n");
        event_free(m_SendDataRandomlyTimer);
        m_SendDataRandomlyTimer = NULL;
        return false;
    }

    return true;
}

void SingleEventBaseClient::CallBack_InputFromCMD(int Input, short events, void *UserData)
{
    char InputMessage[1024];
    memset(InputMessage, 0, sizeof(InputMessage));

    ssize_t ReadSize = read(Input, InputMessage, sizeof(InputMessage));
    if (ReadSize <= 0)
    {
        printf("ERROR: Can not read from cmd.\n");
        return;
    }

    InputMessage[ReadSize - 1] = '\0';
    if (strlen(InputMessage) <= 0)
    {
        return;
    }

    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    if (Client->m_DataBufferevent == NULL || !Client->m_IsConnected)
    {
        printf("Can not send data, not connect server.\n");
        return;
    }

    if (bufferevent_write(Client->m_DataBufferevent, InputMessage, ReadSize) == 0)
    {
        printf("Send message = %s, size = %d.\n", InputMessage, ReadSize);
    }
    else
    {
        printf("Send message = %s failed.\n", InputMessage);
    }
}

void SingleEventBaseClient::CallBack_RecvUDPBroadcast(int Socket, short events, void *UserData)
{
    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    if (Client->m_IsConnected)
    {
        return;
    }

    char UDPBroadcastBuffer[1024];
    memset(UDPBroadcastBuffer, 0, sizeof(UDPBroadcastBuffer));

    socklen_t AddressLength = sizeof(socklen_t);
    ssize_t RecvSize = recvfrom(Client->m_UDPSocket,
        UDPBroadcastBuffer, sizeof(UDPBroadcastBuffer), 0,
        (struct sockaddr *)&(Client->m_BroadcastAddress), &AddressLength);

    if (RecvSize == -1)
    {
        printf("ERROR: UDP broadcast recv error.\n");
        return;
    }

    if (RecvSize == 0)
    {
        printf("ERROR: UDP connection closed.\n");
        return;
    }

    std::string BroadcastMessage(UDPBroadcastBuffer);
    printf("UDP broadcast recv message = %s.\n", UDPBroadcastBuffer);
    std::string::size_type Index = BroadcastMessage.find(":");
    if (Index == std::string::npos)
    {
        printf("ERROR: Can not parse message.\n");
        return;
    }

    Client->m_ServerIP = BroadcastMessage.substr(0, Index);
    Client->m_ServerPort = atoi(BroadcastMessage.substr(Index + 1, BroadcastMessage.size()).c_str());
    printf("Connect Information: Server IP = %s, Port = %d\n", Client->m_ServerIP.c_str(), Client->m_ServerPort);

    if (!Client->ConnectServer(Client->m_ServerIP, Client->m_ServerPort))
    {
        Client->m_ServerPort = 0;
        Client->m_ServerIP.clear();
        printf("ERROR: Can not connect server.\n");
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
        if (bufferevent_write(Client->m_DataBufferevent, UUID.c_str(), UUID.length()) == 0)
        {
            printf("Send = %s, size = %d, succeed.\n", UUID.c_str(), UUID.size());
        }
        else
        {
            printf("Send = %s, size = %d, failed.\n", UUID.c_str(), UUID.size());
        }

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
        printf("error encountered while reading\n");
    }
    else if (Events & BEV_EVENT_WRITING)
    {
        Client->m_IsConnected = false;
        printf("error encountered while writing\n");
    }
    else if (Events & BEV_EVENT_EOF)
    {
        Client->m_IsConnected = false;
        int ClientSocket = bufferevent_getfd(bev);
        printf("Client = %d connection closed.\n", ClientSocket);
    }
    else if (Events & BEV_EVENT_TIMEOUT)
    {
        printf("ERROR: User specified timeout reached.\n");
    }
    else if (Events & BEV_EVENT_CONNECTED)
    {
        Client->m_IsConnected = true;
        printf("Connected server succeed.\n");
        if (Client->m_UDPBroadcastEvent != NULL)
        {
            if (event_del(Client->m_UDPBroadcastEvent) == 0)
            {
                printf("Delete udp broadcast recv event and close udp socket.\n");
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
        printf("unrecoverable error encountered\n");
    }
    else
    {
        Client->m_IsConnected = false;
        printf("ERROR: client event = %d, unknow error.\n", Events);
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
    char ServerMessage[1024];
    memset(ServerMessage, 0, sizeof(ServerMessage));

    size_t RecvSize = bufferevent_read(bev, ServerMessage, sizeof(ServerMessage));
    ServerMessage[RecvSize] = '\0';

    printf("Recv = %s, size = %d\n", ServerMessage, RecvSize);

    SingleEventBaseClient *Client = (SingleEventBaseClient*)UserData;
    Client->ProcessMessage(ServerMessage, RecvSize);
}

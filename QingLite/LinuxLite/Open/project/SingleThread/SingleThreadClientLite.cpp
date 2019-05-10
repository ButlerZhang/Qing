#include "SingleThreadClientLite.h"
#include "../../../LinuxTools.h"
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>



SingleThreadClientLite::SingleThreadClientLite()
{
    m_ServerPort = 0;
    m_UDPSocket = -1;
    m_EventBase = NULL;
    m_Bufferevent = NULL;
    m_CMDInputEvent = NULL;
    m_UDPBroadcastEvent = NULL;
    m_SendDataRandomlyEvent = NULL;
}

SingleThreadClientLite::~SingleThreadClientLite()
{
    event_free(m_CMDInputEvent);
    event_free(m_UDPBroadcastEvent);
    event_free(m_SendDataRandomlyEvent);

    bufferevent_free(m_Bufferevent);
    event_base_free(m_EventBase);
}

bool SingleThreadClientLite::Start()
{
    if (m_EventBase != NULL)
    {
        printf("ERROR: Re-start.\n");
        return true;
    }

    m_EventBase = event_base_new();
    if (m_EventBase == NULL)
    {
        printf("ERROR: Create event base failed.\n");
        return false;
    }

    if (!AddEventInputFromCMD() || !AddEventRecvUDPBroadcast())
    {
        return false;
    }

    printf("Client start dispatch...\n");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleThreadClientLite::Stop()
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

bool SingleThreadClientLite::AddEventInputFromCMD()
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

bool SingleThreadClientLite::AddEventRecvUDPBroadcast()
{
    if (m_UDPBroadcastEvent != NULL)
    {
        printf("ERROR: Re-create udp broadcast event.\n");
        return true;
    }

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
    m_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(12345));

    if (bind(m_UDPSocket, (struct sockaddr *)&m_BroadcastAddress, sizeof(m_BroadcastAddress)) == -1)
    {
        printf("ERROR: Bind udp socket failed.\n");
        return false;
    }

    m_UDPBroadcastEvent = event_new(m_EventBase, m_UDPSocket, EV_READ | EV_PERSIST, CallBack_RecvUDPBroadcast, this);
    if (m_UDPBroadcastEvent == NULL)
    {
        printf("ERROR: Create udp broadcast event failed.\n");
        close(m_UDPSocket);
        return false;
    }

    if (event_add(m_UDPBroadcastEvent, NULL) == -1)
    {
        printf("ERROR: Add udp broadcast event failed.\n");
        event_free(m_UDPBroadcastEvent);
        m_UDPBroadcastEvent = NULL;
        close(m_UDPSocket);
        return false;
    }

    return true;
}

bool SingleThreadClientLite::AddEventSendDataRandomly()
{
    if (m_SendDataRandomlyEvent != NULL)
    {
        printf("ERROR: Re-create send data randomly event.\n");
        return true;
    }

    m_SendDataRandomlyEvent = event_new(m_EventBase, -1, EV_PERSIST, CallBack_SendDataRandomly, this);
    if (m_SendDataRandomlyEvent == NULL)
    {
        printf("ERROR: Create send data randomly event failed.\n");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = GetRandomUIntInRange(3, 10);

    if (event_add(m_SendDataRandomlyEvent, &tv) != 0)
    {
        printf("ERROR: Add send data randomly event failed.\n");
        event_free(m_SendDataRandomlyEvent);
        m_SendDataRandomlyEvent = NULL;
        return false;
    }

    return true;
}

bool SingleThreadClientLite::DeleteEventRecvUDPBroadcast()
{
    if (m_UDPBroadcastEvent == NULL)
    {
        return true;
    }

    if (event_del(m_UDPBroadcastEvent) == 0)
    {
        printf("Delete udp broadcast recv event and close udp socket.\n");
        event_free(m_UDPBroadcastEvent);
        m_UDPBroadcastEvent = NULL;
        close(m_UDPSocket);
        return true;
    }

    printf("ERROR: delete udp broadcast event failed.\n");
    return false;
}

bool SingleThreadClientLite::ConnectServer(const std::string &ServerIP, int Port)
{
    struct sockaddr_in ServerAddress;
    bzero(&ServerAddress, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP.c_str(), &(ServerAddress.sin_addr));
    ServerAddress.sin_port = htons(static_cast<uint16_t>(Port));

    m_Bufferevent = bufferevent_socket_new(m_EventBase, -1, BEV_OPT_CLOSE_ON_FREE);
    if (m_Bufferevent == NULL)
    {
        printf("ERROR: Create bufferevent failed.\n");
        return false;
    }

    int ConnectResult = bufferevent_socket_connect(m_Bufferevent, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
    if (ConnectResult != 0)
    {
        m_ServerPort = 0;
        m_ServerIP.clear();
        bufferevent_free(m_Bufferevent);
        printf("ERROR: bufferevent connect failed.\n");
        return false;
    }

    bufferevent_setcb(m_Bufferevent, CallBack_RecvFromServer, NULL, CallBack_ClientEvent, this);
    bufferevent_enable(m_Bufferevent, EV_READ | EV_PERSIST);
    return true;
}

void SingleThreadClientLite::CallBack_InputFromCMD(int Input, short events, void *UserData)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    ssize_t ReadSize = read(Input, Message, sizeof(Message));
    if (ReadSize <= 0)
    {
        printf("ERROR: Can not read from cmd.\n");
        return;
    }

    Message[ReadSize - 1] = '\0';
    if (strlen(Message) <= 0)
    {
        return;
    }

    SingleThreadClientLite *Client = (SingleThreadClientLite*)UserData;
    if (Client->m_Bufferevent == NULL)
    {
        printf("Can not send data, not connect server.\n");
        return;
    }

    if (bufferevent_write(Client->m_Bufferevent, Message, ReadSize) == 0)
    {
        printf("Send message = %s, size = %d.\n", Message, ReadSize);
    }
    else
    {
        printf("Send message = %s failed.\n", Message, ReadSize);
    }
}

void SingleThreadClientLite::CallBack_RecvUDPBroadcast(int Socket, short events, void *UserData)
{
    char Buffer[1024];
    memset(Buffer, 0, sizeof(Buffer));
    socklen_t AddressLength = sizeof(socklen_t);

    SingleThreadClientLite *Client = (SingleThreadClientLite*)UserData;
    ssize_t RecvSize = recvfrom(Client->m_UDPSocket, Buffer, sizeof(Buffer), 0, (struct sockaddr *)&(Client->m_BroadcastAddress), &AddressLength);

    if (RecvSize == -1)
    {
        printf("ERROR: UDP broadcast recv error.\n");
    }
    else if (RecvSize == 0)
    {
        printf("ERROR: UDP connection closed.\n");
    }
    else
    {
        if (Client->m_ServerIP.empty())
        {
            printf("UDP broadcast recv message = %s.\n", Buffer);

            Client->m_ServerIP = Buffer;
            Client->m_ServerPort = 12345;
            if (Client->ConnectServer(Client->m_ServerIP, Client->m_ServerPort))
            {
                Client->AddEventSendDataRandomly();
                Client->DeleteEventRecvUDPBroadcast();
            }
        }
        else if (Client->m_ServerIP == std::string(Buffer))
        {
            printf("UDP braodcast repeat recv.\n");
        }
    }
}

void SingleThreadClientLite::CallBack_SendDataRandomly(evutil_socket_t Socket, short Events, void *UserData)
{
    SingleThreadClientLite *Client = (SingleThreadClientLite*)UserData;
    if (Client->m_Bufferevent != NULL)
    {
        const std::string &UUID = GetUUID();
        if (bufferevent_write(Client->m_Bufferevent, UUID.c_str(), UUID.length()) == 0)
        {
            printf("Send succeed, message = %s\n", UUID.c_str(), UUID.length());
        }
        else
        {
            printf("Send failed, message = %s\n", UUID.c_str(), UUID.length());
        }

        struct timeval tv;
        evutil_timerclear(&tv);
        tv.tv_sec = GetRandomUIntInRange(3, 10);
        event_add(Client->m_SendDataRandomlyEvent, &tv);
    }
}

void SingleThreadClientLite::CallBack_ClientEvent(struct bufferevent *bev, short Events, void *UserData)
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
        printf("ERROR: User specified timeout reached.\n");
    }
    else if (Events & BEV_EVENT_CONNECTED)
    {
        printf("Connected server succeed.\n");
    }
    else
    {
        IsAllowDelete = true;
        printf("ERROR: unknow error.\n");
    }

    if (IsAllowDelete)
    {
        SingleThreadClientLite *Client = (SingleThreadClientLite*)UserData;

        event_free(Client->m_SendDataRandomlyEvent);
        Client->m_SendDataRandomlyEvent = NULL;

        bufferevent_free(Client->m_Bufferevent);
        Client->m_Bufferevent = NULL;

        if (Client->m_UDPBroadcastEvent == NULL)
        {
            Client->m_ServerIP.clear();
            Client->AddEventRecvUDPBroadcast();
        }
    }
}

void SingleThreadClientLite::CallBack_RecvFromServer(bufferevent * bev, void *UserData)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    size_t RecvSize = bufferevent_read(bev, Message, sizeof(Message));
    Message[RecvSize] = '\0';

    printf("Recv %s.\n", Message);
}

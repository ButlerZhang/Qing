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

    event_base_free(m_EventBase);
}

bool SingleThreadClientLite::Start()
{
    if (m_EventBase != NULL)
    {
        printf("ERROR: re-start.\n");
        return true;
    }

    m_EventBase = event_base_new();
    if (m_EventBase == NULL)
    {
        printf("ERROR: Create event base error.\n");
        return false;
    }

    if (!RecvUDPBroadcast())
    {
        return false;
    }

    printf("Client start dispatch...\n");
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleThreadClientLite::Stop()
{
    if (event_base_loopbreak(m_EventBase) == 0)
    {
        event_base_free(m_EventBase);
        m_EventBase = NULL;
        return true;
    }

    return false;
}

bool SingleThreadClientLite::RecvUDPBroadcast()
{
    m_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_UDPSocket == -1)
    {
        printf("ERROR: Create udp socket error.\n");
        return false;
    }

    int Optval = 1;
    if (setsockopt(m_UDPSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &Optval, sizeof(int)) < 0)
    {
        printf("ERROR: setsockopt failed!\n");
        return false;
    }

    bzero(&m_BroadcastAddress, sizeof(sockaddr_in));
    m_BroadcastAddress.sin_family = AF_INET;
    m_BroadcastAddress.sin_addr.s_addr = INADDR_ANY;
    m_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(12345));

    if (bind(m_UDPSocket, (struct sockaddr *)&m_BroadcastAddress, sizeof(m_BroadcastAddress)) == -1)
    {
        printf("ERROR: Bind failed.\n");
        return false;
    }

    m_UDPBroadcastEvent = event_new(m_EventBase, m_UDPSocket, EV_READ | EV_PERSIST, CallBack_RecvUDPBroadcast, this);
    if (event_add(m_UDPBroadcastEvent, NULL) == -1)
    {
        printf("ERROR: Add event failed.\n");
        return false;
    }

    return true;
}

bool SingleThreadClientLite::UnbindUDPBroadcast()
{
    if (event_del(m_UDPBroadcastEvent) == 0)
    {
        printf("Delete udp broadcast recv event and close udp socket.\n");
        close(m_UDPSocket);

        m_UDPBroadcastEvent = NULL;
        m_UDPSocket = -1;
        return true;
    }

    printf("ERROR: unbind failed.\n");
    return false;
}

bool SingleThreadClientLite::EnableSendDataRandomly()
{
    m_SendDataRandomlyEvent = event_new(m_EventBase, -1, EV_PERSIST, CallBack_SendDataRandomly, this);

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 1;
    event_add(m_SendDataRandomlyEvent, &tv);
    return true;
}

bool SingleThreadClientLite::ConnectServer(const std::string &ServerIP, int Port)
{
    struct sockaddr_in ServerAddress;
    bzero(&ServerAddress, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP.c_str(), &(ServerAddress.sin_addr));
    ServerAddress.sin_port = htons(static_cast<uint16_t>(Port));

    m_Bufferevent = bufferevent_socket_new(m_EventBase, -1, BEV_OPT_CLOSE_ON_FREE);
    int ConnectResult = bufferevent_socket_connect(m_Bufferevent, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));

    if (ConnectResult == -1)
    {
        m_ServerPort = 0;
        m_ServerIP.clear();
        printf("ERROR: bufferevent connect error.\n");
        return false;
    }

    m_CMDInputEvent = event_new(m_EventBase, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_InputFromCMD, this);
    event_add(m_CMDInputEvent, NULL);

    bufferevent_setcb(m_Bufferevent, CallBack_RecvFromServer, NULL, CallBack_ClientEvent, this);
    bufferevent_enable(m_Bufferevent, EV_READ | EV_PERSIST);

    return true;
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
                Client->UnbindUDPBroadcast();
                Client->EnableSendDataRandomly();
            }
        }
        else if (Client->m_ServerIP == std::string(Buffer))
        {
            printf("UDP braodcast repeat recv.\n");
        }
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

        bufferevent_free(Client->m_Bufferevent);
        event_free(Client->m_CMDInputEvent);
        event_free(Client->m_SendDataRandomlyEvent);

        Client->m_Bufferevent = NULL;
        Client->m_CMDInputEvent = NULL;
        Client->m_SendDataRandomlyEvent = NULL;

        if (Client->m_UDPBroadcastEvent == NULL && Client->m_UDPSocket != -1)
        {
            Client->RecvUDPBroadcast();
        }
    }
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
    bufferevent_write(Client->m_Bufferevent, Message, ReadSize);

    printf("Send message = %s, size = %d.\n", Message, ReadSize);
}

void SingleThreadClientLite::CallBack_RecvFromServer(bufferevent * bev, void *UserData)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    size_t RecvSize = bufferevent_read(bev, Message, sizeof(Message));
    Message[RecvSize] = '\0';

    printf("Recv %s.\n", Message);
}

void SingleThreadClientLite::CallBack_SendDataRandomly(evutil_socket_t Socket, short Events, void *UserData)
{
    SingleThreadClientLite *Client = (SingleThreadClientLite*)UserData;
    if (Client->m_Bufferevent != NULL)
    {
        const std::string &UUID = GetUUID();
        if (bufferevent_write(Client->m_Bufferevent, UUID.c_str(), UUID.length()) == 0)
        {
            printf("Send message = %s succeed.\n", UUID.c_str(), UUID.length());
        }
        else
        {
            printf("Send message = %s failed.\n", UUID.c_str(), UUID.length());
        }

        struct timeval tv;
        evutil_timerclear(&tv);
        tv.tv_sec = GetRandomUIntInRange(3, 10);
        event_add(Client->m_SendDataRandomlyEvent, &tv);
    }
}

#include "SingleThreadClientLite.h"
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <uuid/uuid.h>



SingleThreadClientLite::SingleThreadClientLite()
{
    m_ServerPort = 0;
    m_UDPSocket = -1;
    m_EventBase = NULL;
    m_Bufferevent = NULL;
    m_UDPBroadcastEvent = NULL;
    m_SendDataRandomlyEvent = NULL;
}

SingleThreadClientLite::~SingleThreadClientLite()
{
    event_base_free(m_EventBase);
    event_free(m_UDPBroadcastEvent);
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
    return event_base_loopbreak(m_EventBase) == 0;
}

bool SingleThreadClientLite::RecvUDPBroadcast()
{
    m_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_UDPSocket == -1)
    {
        printf("ERROR: Create udp socket error.\n");
        return false;
    }

    int iOptval = 1;
    if (setsockopt(m_UDPSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &iOptval, sizeof(int)) < 0)
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

    struct event *ev_cmd = event_new(m_EventBase, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_InputFromCMD, (void*)m_Bufferevent);
    event_add(ev_cmd, NULL);

    bufferevent_setcb(m_Bufferevent, CallBack_RecvFromServer, NULL, CallBack_ClientEvent, (void*)ev_cmd);
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
                Client->EnableSendDataRandomly();
            }
        }
        else if (Client->m_ServerIP == std::string(Buffer))
        {
            printf("UDP braodcast repeat recv.\n");
            Client->UnbindUDPBroadcast();
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
        bufferevent_free(bev);
        event_free((struct event*)UserData);
    }
}

void SingleThreadClientLite::CallBack_InputFromCMD(int Input, short events, void * UserData)
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

    struct bufferevent *bev = (struct bufferevent*)UserData;
    bufferevent_write(bev, Message, ReadSize);

    printf("Send message = %s, size = %d.\n", Message, ReadSize);
}

void SingleThreadClientLite::CallBack_RecvFromServer(bufferevent * bev, void * UserData)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    size_t RecvSize = bufferevent_read(bev, Message, sizeof(Message));
    Message[RecvSize] = '\0';

    printf("Recv message = %s, size = %d.\n", Message, RecvSize);
}

void SingleThreadClientLite::CallBack_SendDataRandomly(evutil_socket_t Socket, short Events, void *UserData)
{
    uuid_t uuid;
    char uuidstring[36];

    uuid_generate(uuid);
    uuid_unparse(uuid, uuidstring);

    SingleThreadClientLite *Client = (SingleThreadClientLite*)UserData;
    bufferevent_write(Client->m_Bufferevent, uuidstring, sizeof(uuidstring));
    printf("Send message = %s, size = %d.\n", uuidstring, sizeof(uuidstring));

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = rand() % 1 + 1;
    event_add(Client->m_SendDataRandomlyEvent, &tv);
}

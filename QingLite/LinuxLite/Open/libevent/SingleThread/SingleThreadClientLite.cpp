#include "SingleThreadClientLite.h"
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>



SingleThreadClientLite::SingleThreadClientLite()
{
    m_ServerPort = 0;
    m_EventBase = event_base_new();
}

SingleThreadClientLite::~SingleThreadClientLite()
{
    event_base_free(m_EventBase);
}

bool SingleThreadClientLite::Start()
{
    RecvUDPBroadcast();
    event_base_dispatch(m_EventBase);
    return true;
}

bool SingleThreadClientLite::Stop()
{
    return false;
}

bool SingleThreadClientLite::RecvUDPBroadcast()
{
    m_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_UDPSocket == -1)
    {
        printf("ERROR: Create udp socket error.\n\n");
        return false;
    }

    int iOptval = 1;
    if (setsockopt(m_UDPSocket, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &iOptval, sizeof(int)) < 0)
    {
        printf("ERROR: setsockopt failed!");
        return false;
    }

    bzero(&m_BroadcastAddress, sizeof(sockaddr_in));
    m_BroadcastAddress.sin_family = AF_INET;
    m_BroadcastAddress.sin_addr.s_addr = INADDR_ANY;
    m_BroadcastAddress.sin_port = htons(static_cast<uint16_t>(12345));

    if (bind(m_UDPSocket, (struct sockaddr *)&m_BroadcastAddress, sizeof(m_BroadcastAddress)) == -1)
    {
        printf("bind failed!\n");
        return false;
    }

    struct event *UDPEvent = event_new(m_EventBase, m_UDPSocket, EV_READ | EV_PERSIST, CallBack_UDPBroadcastRecv, this);
    if (event_add(UDPEvent, NULL) == -1)
    {
        printf("event add failed\n");
        return false;
    }

    return true;
}

bool SingleThreadClientLite::ConnectServer(const std::string &ServerIP, int Port)
{
    struct sockaddr_in ServerAddress;
    bzero(&ServerAddress, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP.c_str(), &(ServerAddress.sin_addr));
    ServerAddress.sin_port = htons(static_cast<uint16_t>(Port));

    struct bufferevent *bev = bufferevent_socket_new(m_EventBase, -1, BEV_OPT_CLOSE_ON_FREE);
    int ConnectResult = bufferevent_socket_connect(bev, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));

    if (ConnectResult == -1)
    {
        m_ServerPort = 0;
        m_ServerIP.clear();
        printf("bufferevent connect error.\n");
        return false;
    }

    struct event *ev_cmd = event_new(m_EventBase, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack_InputFromCMD, (void*)bev);
    event_add(ev_cmd, NULL);

    bufferevent_setcb(bev, CallBack_RecvFromServer, NULL, CallBack_ClientEvent, (void*)ev_cmd);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    return true;
}

void SingleThreadClientLite::CallBack_UDPBroadcastRecv(int Socket, short events, void *UserData)
{
    char Buffer[1024];
    memset(Buffer, 0, sizeof(Buffer));
    socklen_t AddressLength = sizeof(socklen_t);

    SingleThreadClientLite *Client = (SingleThreadClientLite*)UserData;
    ssize_t RecvSize = recvfrom(Client->m_UDPSocket, Buffer, sizeof(Buffer), 0, (struct sockaddr *)&(Client->m_BroadcastAddress), &AddressLength);

    if (RecvSize == -1)
    {
        printf("Recv error.\n\n");
        return;
    }

    if (RecvSize == 0)
    {
        printf("Connection closed.\n");
        return;
    }

    if (Client->m_ServerIP.empty())
    {
        printf("Recv message = %s.\n\n", Buffer);
        Client->m_ServerIP = Buffer;
        Client->m_ServerPort = 12345;
        Client->ConnectServer(Client->m_ServerIP, Client->m_ServerPort);
        return;
    }

    if (Client->m_ServerIP == std::string(Buffer))
    {
        printf("Repeat recv.\n\n");
    }
}

void SingleThreadClientLite::CallBack_ClientEvent(struct bufferevent *bev, short events, void *UserData)
{
    if (events & BEV_EVENT_CONNECTED)
    {
        printf("Connected server succeed.\n\n");
        return;
    }
    else if (events & BEV_EVENT_EOF)
    {
        printf("Connection closed.\n\n");
    }
    else if (events & BEV_EVENT_ERROR)
    {
        printf("Unknow error.\n\n");
    }

    bufferevent_free(bev);
    event_free((struct event*)UserData);
}

void SingleThreadClientLite::CallBack_InputFromCMD(int Input, short events, void * UserData)
{
    char Message[1024];
    memset(Message, 0, sizeof(Message));

    ssize_t ReadSize = read(Input, Message, sizeof(Message));
    if (ReadSize <= 0)
    {
        printf("ERROR: Can not read from cmd.\n\n");
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

    printf("Recv message = %s, size = %d.\n\n", Message, RecvSize);
}

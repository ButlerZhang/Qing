#include "ServerLite.h"
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>



ServerLite::ServerLite()
{
}

ServerLite::~ServerLite()
{
    m_ThreadVector.clear();
}

bool ServerLite::StartServer(const std::string &IP, int Port, int ThreadCount)
{
    m_MainThread.m_ThreadID = pthread_self();
    m_MainThread.m_EventBase = event_base_new();
    if (m_MainThread.m_EventBase == NULL)
    {
        printf("Create main event base error.\n");
        return false;
    }

    if (!CreateThreads(ThreadCount))
    {
        printf("Create threads error.\n");
        return false;
    }

    if (!StartListen(IP, Port))
    {
        printf("Start listen error.\n");
        return false;
    }

    return true;
}

void ServerLite::StopServer()
{
    int EXIT_CODE = -1;
    for (std::vector<ThreadNode>::size_type Index = 0; Index < m_ThreadVector.size(); Index++)
    {
        write(m_ThreadVector[Index].m_NotifySendPipeFD, &EXIT_CODE, sizeof(int));
    }

    event_base_loopexit(m_MainThread.m_EventBase, NULL);

    event_base_free(m_MainThread.m_EventBase);
    for (std::vector<ThreadNode>::size_type Index = 0; Index < m_ThreadVector.size(); Index++)
    {
        event_base_free(m_ThreadVector[Index].m_EventBase);
    }
}

bool ServerLite::ProcessConnect(ConnectNode & ConnectedNode)
{
    printf("Client socket = %d connected.\n", ConnectedNode.m_ClientSocket);
    return true;
}

bool ServerLite::ProcessRecv(ConnectNode & ConnectedNode)
{
    if (evbuffer_get_length(ConnectedNode.m_ReadBuffer) <= 0)
    {
        printf("Process recv, no data.\n\n");
        return false;
    }

    char Message[1024];
    memset(Message, 0, sizeof(Message));

    int RecvSize = evbuffer_remove(ConnectedNode.m_ReadBuffer, Message, sizeof(Message));
    Message[RecvSize] = '\0';

    printf("Client = %d recv = %s, size = %d.\n", ConnectedNode.m_ClientSocket, Message, RecvSize);
    return true;

    const std::string ACK("ACK");
    evbuffer_add(ConnectedNode.m_WriteBuffer, ACK.c_str(), ACK.length());
    printf("Client = %d send ack, size = %d.\n\n", ConnectedNode.m_ClientSocket, ACK.length());
    return true;
}

bool ServerLite::ProcessSend(ConnectNode & ConnectedNode)
{
    if (evbuffer_get_length(ConnectedNode.m_WriteBuffer) <= 0)
    {
        printf("Process send, no data.\n\n");
        return false;
    }

    printf("Client = %d send data.\n", ConnectedNode.m_ClientSocket);
    return true;
}

bool ServerLite::ProcessClose(ConnectNode & ConnectedNode, short events)
{
    printf("Client socket = %d closed.\n", ConnectedNode.m_ClientSocket);
    return true;
}

bool ServerLite::CreateThreads(int ThreadCount)
{
    if (ThreadCount <= 0)
    {
        ThreadCount = static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
    }

    for (int Index = 0; Index < ThreadCount; Index++)
    {
        m_ThreadVector.push_back(ThreadNode());
        ThreadNode &NewNode = m_ThreadVector[Index];

        NewNode.m_EventBase = event_base_new();
        if (NewNode.m_EventBase == NULL)
        {
            printf("Create thread event base error.\n");
            return false;
        }

        int ThreadPipe[2];
        if (pipe(ThreadPipe) == -1)
        {
            printf("Create pipe error.\n");
            return false;
        }

        NewNode.m_NotifyRecvPipeFD = ThreadPipe[0];
        NewNode.m_NotifySendPipeFD = ThreadPipe[1];
        NewNode.m_Server = this;

        event_set(&NewNode.m_NotifyEvent, NewNode.m_NotifyRecvPipeFD, EV_READ | EV_PERSIST, CallBack_Accept, &NewNode);
        event_base_set(NewNode.m_EventBase, &NewNode.m_NotifyEvent);
        if (event_add(&NewNode.m_NotifyEvent, 0) == -1)
        {
            printf("Add thread event error.\n");
            return false;
        }
    }

    printf("Create %d threads succeed.\n", static_cast<int>(m_ThreadVector.size()));
    return true;
}

bool ServerLite::StartListen(const std::string & IP, int Port)
{
    sockaddr_in BindAddress;
    bzero(&BindAddress, sizeof(sockaddr_in));
    BindAddress.sin_family = AF_INET;
    BindAddress.sin_port = htons(static_cast<uint16_t>(Port));
    if (!IP.empty())
    {
        inet_pton(AF_INET, IP.c_str(), &(BindAddress.sin_addr));
    }

    evconnlistener * Listener = evconnlistener_new_bind(
        m_MainThread.m_EventBase,
        CallBack_Listen,
        (void*)this,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        5,
        (sockaddr*)&BindAddress,
        sizeof(sockaddr_in));
    if (Listener == NULL)
    {
        printf("Create listener error.\n");
        return false;
    }

    return StartEventLoop(Listener);
}

bool ServerLite::StartEventLoop(evconnlistener *Listener)
{
    for (std::vector<ThreadNode>::size_type Index = 0; Index < m_ThreadVector.size(); Index++)
    {
        ThreadNode &NewNode = m_ThreadVector[Index];
        pthread_create(&NewNode.m_ThreadID, NULL, CallBack_StartThreadEventLoop, (void*)&NewNode);
    }

    event_base_dispatch(m_MainThread.m_EventBase);
    evconnlistener_free(Listener);
    return true;
}

void ServerLite::CallBack_Listen(evconnlistener * Listener, evutil_socket_t Socket, sockaddr *sa, int socklen, void *user_data)
{
    ServerLite *Server = (ServerLite*)user_data;
    std::vector<ThreadNode>::size_type Index = rand() % Server->m_ThreadVector.size();

    int Pipe = Server->m_ThreadVector[Index].m_NotifySendPipeFD;
    write(Pipe, &Socket, sizeof(evutil_socket_t));
}

void ServerLite::CallBack_Accept(int fd, short which, void *arg)
{
    ThreadNode *CurrentNode = (ThreadNode*)arg;

    evutil_socket_t ClientSocket;
    read(CurrentNode->m_NotifyRecvPipeFD, &ClientSocket, sizeof(evutil_socket_t));
    if (ClientSocket <= 0)
    {
        printf("Accept: loop break, client socket = %d.\n", ClientSocket);
        event_base_loopbreak(CurrentNode->m_EventBase);
        return;
    }

    printf("Accept: read socket = %d from recv pipe.\n", ClientSocket);

    struct bufferevent *bev = bufferevent_socket_new(CurrentNode->m_EventBase, ClientSocket, BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL)
    {
        printf("Accept: could not create bufferevent.\n");
        event_base_loopbreak(CurrentNode->m_EventBase);
        return;
    }

    CurrentNode->m_ConnectNodeVector.push_back(ConnectNode());
    ConnectNode &NewConnectedNode = CurrentNode->m_ConnectNodeVector[CurrentNode->m_ConnectNodeVector.size() - 1];
    NewConnectedNode.m_ClientSocket = ClientSocket;
    NewConnectedNode.m_WorkThread = CurrentNode;

    bufferevent_setcb(bev, CallBack_Recv, CallBack_Send, CallBack_Close, &NewConnectedNode);
    bufferevent_enable(bev, EV_WRITE | EV_PERSIST);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    CurrentNode->m_Server->ProcessConnect(NewConnectedNode);
}

void ServerLite::CallBack_Recv(bufferevent * bev, void *data)
{
    ConnectNode *CurrentNode = (ConnectNode*)data;
    CurrentNode->m_ReadBuffer = bufferevent_get_input(bev);
    CurrentNode->m_WriteBuffer = bufferevent_get_output(bev);
    CurrentNode->m_WorkThread->m_Server->ProcessRecv(*CurrentNode);
}

void ServerLite::CallBack_Send(bufferevent * bev, void * data)
{
    ConnectNode *CurrentNode = (ConnectNode*)data;
    CurrentNode->m_ReadBuffer = bufferevent_get_input(bev);
    CurrentNode->m_WriteBuffer = bufferevent_get_output(bev);
    CurrentNode->m_WorkThread->m_Server->ProcessSend(*CurrentNode);
}

void ServerLite::CallBack_Close(bufferevent * bev, short events, void * data)
{
    ConnectNode *CurrentNode = (ConnectNode*)data;
    CurrentNode->m_WorkThread->m_Server->ProcessClose(*CurrentNode, events);

    std::vector<ConnectNode> &NodeVector = CurrentNode->m_WorkThread->m_ConnectNodeVector;
    for (std::vector<ConnectNode>::iterator it = NodeVector.begin(); it != NodeVector.end(); it++)
    {
        if (it->m_ClientSocket == CurrentNode->m_ClientSocket)
        {
            NodeVector.erase(it);
            break;
        }
    }

    bufferevent_free(bev);
}

void* ServerLite::CallBack_StartThreadEventLoop(void *arg)
{
    ThreadNode *CurrentNode = (ThreadNode*)arg;
    printf("Thread %u started.\n", CurrentNode->m_ThreadID);

    event_base_dispatch(CurrentNode->m_EventBase);
    printf("Thread %u done.\n", CurrentNode->m_ThreadID);
    return NULL;
}

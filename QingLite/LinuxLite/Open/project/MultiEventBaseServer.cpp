#include "MultiEventBaseServer.h"
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>



MultiEventBaseServer::MultiEventBaseServer()
{
}

MultiEventBaseServer::~MultiEventBaseServer()
{
    m_ThreadVector.clear();
}

bool MultiEventBaseServer::Start(const std::string &IP, int Port, int ThreadCount)
{
    if (m_MainThread.m_EventBase != NULL)
    {
        printf("ERROR: Re-start.\n");
        return true;
    }

    m_MainThread.m_ThreadID = pthread_self();
    m_MainThread.m_EventBase = event_base_new();
    if (m_MainThread.m_EventBase == NULL)
    {
        printf("ERROR: Create main event base failed.\n");
        return false;
    }

    if (!m_UDPBroadcast.BindBaseEvent(m_MainThread.m_EventBase))
    {
        printf("ERROR: UDP braodcast bind event base failed.\n");
        return false;
    }

    m_UDPBroadcast.StartTimer(IP, 10, Port);

    if (!CreateThreads(ThreadCount))
    {
        printf("ERROR: Create threads failed.\n");
        return false;
    }

    if (!StartListen(IP, Port))
    {
        printf("ERROR: Start listen failed.\n");
        return false;
    }

    return true;
}

void MultiEventBaseServer::Stop()
{
    int EXIT_CODE = -1;
    for (std::vector<ThreadNode>::size_type Index = 0; Index < m_ThreadVector.size(); Index++)
    {
        write(m_ThreadVector[Index].m_NotifySendPipeFD, &EXIT_CODE, sizeof(int));
    }

    event_base_loopexit(m_MainThread.m_EventBase, NULL);
    event_base_free(m_MainThread.m_EventBase);
    m_MainThread.m_EventBase = NULL;

    for (std::vector<ThreadNode>::size_type Index = 0; Index < m_ThreadVector.size(); Index++)
    {
        event_base_free(m_ThreadVector[Index].m_EventBase);
        m_ThreadVector[Index].m_EventBase = NULL;
    }
}

bool MultiEventBaseServer::ProcessConnect(ConnectNode &ConnectedNode)
{
    printf("Client socket = %d connected.\n", ConnectedNode.m_ClientSocket);
    return true;
}

bool MultiEventBaseServer::ProcessRecv(ConnectNode & ConnectedNode)
{
    if (evbuffer_get_length(ConnectedNode.m_ReadBuffer) <= 0)
    {
        printf("Process recv, no data.\n\n");
        return false;
    }

    char RecvBuffer[1024];
    memset(RecvBuffer, 0, sizeof(RecvBuffer));

    int RecvSize = evbuffer_remove(ConnectedNode.m_ReadBuffer, RecvBuffer, sizeof(RecvBuffer));
    RecvBuffer[RecvSize] = '\0';

    printf("Client = %d recv = %s, size = %d.\n", ConnectedNode.m_ClientSocket, RecvBuffer, RecvSize);
    return true;

    //const std::string ACK("ACK");
    //evbuffer_add(ConnectedNode.m_WriteBuffer, ACK.c_str(), ACK.length());
    //printf("Client = %d send ack, size = %d.\n\n", ConnectedNode.m_ClientSocket, ACK.length());
    //return true;
}

bool MultiEventBaseServer::ProcessSend(ConnectNode & ConnectedNode)
{
    if (evbuffer_get_length(ConnectedNode.m_WriteBuffer) <= 0)
    {
        printf("Process send, no data.\n");
        return false;
    }

    printf("Client = %d send data.\n", ConnectedNode.m_ClientSocket);
    return true;
}

bool MultiEventBaseServer::ProcessClose(ConnectNode &ConnectedNode, short events)
{
    printf("Client socket = %d closed.\n", ConnectedNode.m_ClientSocket);
    return true;
}

bool MultiEventBaseServer::CreateThreads(int ThreadCount)
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
            printf("ERROR: Create thread event base error.\n");
            return false;
        }

        int ThreadPipe[2];
        if (pipe(ThreadPipe) == -1)
        {
            printf("ERROR: Create pipe error.\n");
            return false;
        }

        NewNode.m_NotifyRecvPipeFD = ThreadPipe[0];
        NewNode.m_NotifySendPipeFD = ThreadPipe[1];
        NewNode.m_MultiEventBaseServer = this;

        event_set(&NewNode.m_NotifyEvent, NewNode.m_NotifyRecvPipeFD, EV_READ | EV_PERSIST, CallBack_Accept, &NewNode);
        event_base_set(NewNode.m_EventBase, &NewNode.m_NotifyEvent);
        if (event_add(&NewNode.m_NotifyEvent, 0) == -1)
        {
            printf("ERROR: Add thread event error.\n");
            return false;
        }
    }

    printf("Create %d threads succeed.\n", static_cast<int>(m_ThreadVector.size()));
    return !m_ThreadVector.empty();
}

bool MultiEventBaseServer::StartListen(const std::string & IP, int Port)
{
    sockaddr_in BindAddress;
    bzero(&BindAddress, sizeof(sockaddr_in));
    BindAddress.sin_family = AF_INET;
    BindAddress.sin_port = htons(static_cast<uint16_t>(Port));
    if (!IP.empty())
    {
        inet_pton(AF_INET, IP.c_str(), &(BindAddress.sin_addr));
        printf("Server bind IP = %s, port = %d.\n", IP.c_str(), Port);
    }
    else
    {
        BindAddress.sin_addr.s_addr = INADDR_ANY;
        printf("Server bind any IP, port = %d.\n", Port);
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
        printf("ERROR: Create listener error.\n");
        return false;
    }

    return StartEventLoop(Listener);
}

bool MultiEventBaseServer::StartEventLoop(evconnlistener *Listener)
{
    if (m_MainThread.m_EventBase == NULL || m_ThreadVector.empty())
    {
        return false;
    }

    for (std::vector<ThreadNode>::size_type Index = 0; Index < m_ThreadVector.size(); Index++)
    {
        ThreadNode &NewNode = m_ThreadVector[Index];
        pthread_create(&NewNode.m_ThreadID, NULL, CallBack_StartThreadEventLoop, (void*)&NewNode);
    }

    event_base_dispatch(m_MainThread.m_EventBase);
    evconnlistener_free(Listener);
    return true;
}

void MultiEventBaseServer::CallBack_Listen(evconnlistener * Listener, int Socket, sockaddr *sa, int socklen, void *user_data)
{
    MultiEventBaseServer *Server = (MultiEventBaseServer*)user_data;
    const std::vector<ThreadNode> &NodeVector = Server->m_ThreadVector;

    std::vector<ThreadNode>::size_type TargetIndex = 0;
    std::vector<ThreadNode>::size_type ConnectedNodeCount = __UINT32_MAX__;
    for (std::vector<ThreadNode>::size_type Index = 0; Index < NodeVector.size(); Index++)
    {
        if (ConnectedNodeCount < NodeVector[Index].m_ConnectNodeVector.size())
        {
            ConnectedNodeCount = NodeVector[Index].m_ConnectNodeVector.size();
            TargetIndex = Index;
        }
    }

    if (TargetIndex < 0 || TargetIndex >= NodeVector.size())
    {
        TargetIndex = rand() % NodeVector.size();
    }

    int Pipe = Server->m_ThreadVector[TargetIndex].m_NotifySendPipeFD;
    write(Pipe, &Socket, sizeof(evutil_socket_t));

    printf("Listen callback: Client = %d, index = %d, pipe = %d, thread = %u.\n",
        Socket, TargetIndex, Pipe, Server->m_ThreadVector[TargetIndex].m_ThreadID);
}

void MultiEventBaseServer::CallBack_Accept(int fd, short which, void *arg)
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
        return;
    }

    CurrentNode->m_ConnectNodeVector.push_back(ConnectNode());
    ConnectNode &NewConnectedNode = CurrentNode->m_ConnectNodeVector[CurrentNode->m_ConnectNodeVector.size() - 1];
    NewConnectedNode.m_ClientSocket = ClientSocket;
    NewConnectedNode.m_WorkThread = CurrentNode;

    bufferevent_setcb(bev, CallBack_Recv, CallBack_Send, CallBack_Event, &NewConnectedNode);
    bufferevent_enable(bev, EV_WRITE | EV_PERSIST);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    CurrentNode->m_MultiEventBaseServer->ProcessConnect(NewConnectedNode);
}

void MultiEventBaseServer::CallBack_Recv(bufferevent * bev, void *data)
{
    ConnectNode *CurrentNode = (ConnectNode*)data;
    CurrentNode->m_ReadBuffer = bufferevent_get_input(bev);
    CurrentNode->m_WriteBuffer = bufferevent_get_output(bev);
    CurrentNode->m_WorkThread->m_MultiEventBaseServer->ProcessRecv(*CurrentNode);
}

void MultiEventBaseServer::CallBack_Send(bufferevent * bev, void * data)
{
    ConnectNode *CurrentNode = (ConnectNode*)data;
    CurrentNode->m_ReadBuffer = bufferevent_get_input(bev);
    CurrentNode->m_WriteBuffer = bufferevent_get_output(bev);
    CurrentNode->m_WorkThread->m_MultiEventBaseServer->ProcessSend(*CurrentNode);
}

void MultiEventBaseServer::CallBack_Event(bufferevent * bev, short events, void * data)
{
    ConnectNode *CurrentNode = (ConnectNode*)data;
    CurrentNode->m_WorkThread->m_MultiEventBaseServer->ProcessClose(*CurrentNode, events);

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
    printf("Surplus client count = %d.\n", NodeVector.size());
}

void* MultiEventBaseServer::CallBack_StartThreadEventLoop(void *arg)
{
    ThreadNode *CurrentNode = (ThreadNode*)arg;
    printf("Thread %u started.\n", CurrentNode->m_ThreadID);

    event_base_dispatch(CurrentNode->m_EventBase);
    printf("Thread %u done.\n", CurrentNode->m_ThreadID);
    return NULL;
}

#include "MultiEventBaseServer.h"
#include "../../../../Share/Boost/BoostLog.h"
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
        g_Log.WriteError("Multi server Re-start.");
        return true;
    }

    m_MainThread.m_ThreadID = pthread_self();
    m_MainThread.m_EventBase = event_base_new();
    if (m_MainThread.m_EventBase == NULL)
    {
        g_Log.WriteError("Create main event base failed.");
        return false;
    }

    m_UDPBroadcast.StartTimer(m_MainThread.m_EventBase, IP, 10, Port);

    if (!CreateThreads(ThreadCount))
    {
        g_Log.WriteError("Create threads failed.");
        return false;
    }

    if (!StartListen(IP, Port))
    {
        g_Log.WriteError("Start listen failed.");
        return false;
    }

    g_Log.WriteInfo("Multi Server start dispatch...");
    return true;
}

void MultiEventBaseServer::Stop()
{
    int EXIT_CODE = -1;
    for (std::vector<ThreadNode>::size_type Index = 0; Index < m_ThreadVector.size(); Index++)
    {
        if (write(m_ThreadVector[Index].m_NotifySendPipeFD, (const void*)&EXIT_CODE, sizeof(int)) != sizeof(int))
        {
            //add log
        }
    }

    event_base_loopexit(m_MainThread.m_EventBase, NULL);
    event_base_free(m_MainThread.m_EventBase);
    m_MainThread.m_EventBase = NULL;

    for (std::vector<ThreadNode>::size_type Index = 0; Index < m_ThreadVector.size(); Index++)
    {
        event_base_free(m_ThreadVector[Index].m_EventBase);
        m_ThreadVector[Index].m_EventBase = NULL;
    }

    g_Log.WriteInfo("Multi server stop.");
}

bool MultiEventBaseServer::ProcessConnected(ConnectNode &ConnectedNode)
{
    g_Log.WriteDebug(BoostFormat("Client socket = %d connected.", ConnectedNode.m_ClientSocket));
    return true;
}

bool MultiEventBaseServer::ProcessRecv(ConnectNode &ConnectedNode)
{
    if (evbuffer_get_length(ConnectedNode.m_ReadBuffer) <= 0)
    {
        g_Log.WriteDebug("Process recv, no data.");
        return false;
    }

    std::vector<char> ClientMessage(NETWORK_BUFFER_SIZE, 0);
    int RecvSize = evbuffer_remove(ConnectedNode.m_ReadBuffer, &ClientMessage[0], ClientMessage.size());
    g_Log.WriteDebug(BoostFormat("Client = %d recv message size = %d.", ConnectedNode.m_ClientSocket, RecvSize));

    ConnectedNode.m_Message.assign(ClientMessage.begin(), ClientMessage.begin() + RecvSize);
    return ProcessMessage(ConnectedNode);
}

bool MultiEventBaseServer::ProcessSend(ConnectNode &ConnectedNode)
{
    if (evbuffer_get_length(ConnectedNode.m_WriteBuffer) <= 0)
    {
        return true;
    }

    g_Log.WriteDebug(BoostFormat("Client = %d send data.", ConnectedNode.m_ClientSocket));
    return true;
}

bool MultiEventBaseServer::ProcessDisconnected(ConnectNode &ConnectedNode, short events)
{
    g_Log.WriteDebug(BoostFormat("Client socket = %d closed.\n", ConnectedNode.m_ClientSocket));
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
            g_Log.WriteError("Create thread event base error.");
            return false;
        }

        int ThreadPipe[2];
        if (pipe(ThreadPipe) == -1)
        {
            g_Log.WriteError("Create pipe error.");
            return false;
        }

        NewNode.m_NotifyRecvPipeFD = ThreadPipe[0];
        NewNode.m_NotifySendPipeFD = ThreadPipe[1];
        NewNode.m_MultiEventBaseServer = this;
    }

    //对于容器，不能一边push新元素，一边引用新元素的地址
    for (int Index = 0; Index < ThreadCount; Index++)
    {
        ThreadNode &NewNode = m_ThreadVector[Index];
        event_set(&NewNode.m_NotifyEvent, NewNode.m_NotifyRecvPipeFD, EV_READ | EV_PERSIST, CallBack_Accept, &NewNode);
        event_base_set(NewNode.m_EventBase, &NewNode.m_NotifyEvent);
        if (event_add(&NewNode.m_NotifyEvent, 0) == -1)
        {
            g_Log.WriteError("Add thread event error.");
            return false;
        }
    }

    g_Log.WriteInfo(BoostFormat("Create %d threads succeed.", static_cast<int>(m_ThreadVector.size())));
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
        g_Log.WriteInfo(BoostFormat("Server bind IP = %s, port = %d.", IP.c_str(), Port));
    }
    else
    {
        BindAddress.sin_addr.s_addr = INADDR_ANY;
        g_Log.WriteInfo(BoostFormat("Server bind any IP, port = %d.", Port));
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
        g_Log.WriteError("Create listener error.");
        return false;
    }

    return StartEventLoop(Listener);
}

bool MultiEventBaseServer::StartEventLoop(evconnlistener *Listener)
{
    if (m_MainThread.m_EventBase == NULL || m_ThreadVector.empty())
    {
        g_Log.WriteError("Start event loop failed, main event base is NULL or no thread.");
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

bool MultiEventBaseServer::Send(ConnectNode & ConnectedNode, const std::string & DataString)
{
    int AddSize = evbuffer_add(ConnectedNode.m_WriteBuffer, DataString.c_str(), DataString.size());
    if (AddSize != static_cast<int>(DataString.size()))
    {
        g_Log.WriteError("Send falied, evbuffer add error.");
        return false;
    }

    int WriteSize = evbuffer_write(ConnectedNode.m_WriteBuffer, ConnectedNode.m_ClientSocket);
    if (WriteSize != static_cast<int>(DataString.size()))
    {
        g_Log.WriteError("Send falied, evbuffer write error.");
        return false;
    }

    g_Log.WriteError(BoostFormat("Client = %d send size = %d succeed.", ConnectedNode.m_ClientSocket, WriteSize));
    return true;
}

void MultiEventBaseServer::CallBack_Listen(evconnlistener * Listener, int Socket, sockaddr *sa, int socklen, void *user_data)
{
    MultiEventBaseServer *Server = (MultiEventBaseServer*)user_data;
    const std::vector<ThreadNode> &ThreadVector = Server->m_ThreadVector;

    std::vector<ThreadNode>::size_type TargetIndex = 0;
    std::vector<ThreadNode>::size_type ConnectedNodeCount = __INT32_MAX__;
    for (std::vector<ThreadNode>::size_type Index = 0; Index < ThreadVector.size(); Index++)
    {
        if (ConnectedNodeCount > ThreadVector[Index].m_ConnectNodeVector.size())
        {
            ConnectedNodeCount = ThreadVector[Index].m_ConnectNodeVector.size();
            TargetIndex = Index;
        }
    }

    if (TargetIndex < 0 || TargetIndex >= ThreadVector.size())
    {
        TargetIndex = rand() % ThreadVector.size();
        g_Log.WriteError("I should not come here to find target index.");
    }

    int Pipe = Server->m_ThreadVector[TargetIndex].m_NotifySendPipeFD;
    if (write(Pipe, (const void*)&Socket, sizeof(evutil_socket_t)) != sizeof(evutil_socket_t))
    {
        //add log
    }

    g_Log.WriteInfo(BoostFormat("Listen callback: Client = %d, index = %d, pipe = %d, thread = %u.\n",
        Socket, TargetIndex, Pipe, Server->m_ThreadVector[TargetIndex].m_ThreadID));
}

void MultiEventBaseServer::CallBack_Accept(int fd, short which, void *arg)
{
    ThreadNode *CurrentThreadNode = (ThreadNode*)arg;

    evutil_socket_t ClientSocket;
    ssize_t ReadSize = read(CurrentThreadNode->m_NotifyRecvPipeFD, &ClientSocket, sizeof(evutil_socket_t));
    if (ReadSize <= 0 || ClientSocket <= 0)
    {
        g_Log.WriteError(BoostFormat("Accept: loop break, client socket = %d.\n", ClientSocket));
        event_base_loopbreak(CurrentThreadNode->m_EventBase);
        return;
    }

    g_Log.WriteInfo(BoostFormat("Accept: read socket = %d from recv pipe.\n", ClientSocket));

    struct bufferevent *bev = bufferevent_socket_new(CurrentThreadNode->m_EventBase, ClientSocket, BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL)
    {
        g_Log.WriteError("Accept: could not create bufferevent.");
        return;
    }

    CurrentThreadNode->m_ConnectNodeVector.push_back(ConnectNode());
    ConnectNode &NewConnectedNode = CurrentThreadNode->m_ConnectNodeVector[CurrentThreadNode->m_ConnectNodeVector.size() - 1];
    NewConnectedNode.m_ClientSocket = ClientSocket;
    NewConnectedNode.m_WorkThread = CurrentThreadNode;

    bufferevent_setcb(bev, CallBack_Recv, CallBack_Send, CallBack_Event, &NewConnectedNode);
    bufferevent_enable(bev, EV_WRITE | EV_PERSIST);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    CurrentThreadNode->m_MultiEventBaseServer->ProcessConnected(NewConnectedNode);
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
    CurrentNode->m_WorkThread->m_MultiEventBaseServer->ProcessDisconnected(*CurrentNode, events);

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
    g_Log.WriteInfo(BoostFormat("Thread = %u surplus client count = %d.\n", CurrentNode->m_WorkThread->m_ThreadID, NodeVector.size()));
}

void* MultiEventBaseServer::CallBack_StartThreadEventLoop(void *arg)
{
    ThreadNode *CurrentNode = (ThreadNode*)arg;
    g_Log.WriteInfo(BoostFormat("Thread = %u started.\n", CurrentNode->m_ThreadID));

    event_base_dispatch(CurrentNode->m_EventBase);
    g_Log.WriteInfo(BoostFormat("Thread = %u done.\n", CurrentNode->m_ThreadID));
    return NULL;
}

#pragma once
#include <vector>
#include <event.h>
#include <pthread.h>

struct ThreadNode;



struct ConnectNode
{
    int                      m_ClientSocket;
    evbuffer                *m_ReadBuffer;
    evbuffer                *m_WriteBuffer;
    ThreadNode              *m_WorkThread;
};

struct ThreadNode
{
    pthread_t                m_ThreadID;
    int                      m_NotifyRecvPipeFD;
    int                      m_NotifySendPipeFD;

    struct event_base       *m_EventBase;
    struct event             m_NotifyEvent;

    struct ServerLite       *m_Server;
    std::vector<ConnectNode> m_ConnectNodeVector;
};

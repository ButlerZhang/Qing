#pragma once
#include <string>
#include <vector>
#include <event.h>
#include <pthread.h>



struct ThreadNode;
const int NETWORK_BUFFER_SIZE = 2048;



struct NetworkMessage
{
    int                                      m_Socket;
    std::string                              m_Message;
    struct bufferevent                      *m_Bufferevent;

    NetworkMessage() : m_Socket(0), m_Bufferevent(NULL) {}
};



struct ConnectNode
{
    int                                  m_ClientSocket;
    evbuffer                            *m_ReadBuffer;
    evbuffer                            *m_WriteBuffer;
    ThreadNode                          *m_WorkThread;
    std::string                          m_Message;

    ConnectNode()
    {
        m_ClientSocket = -1;
        m_ReadBuffer = NULL;
        m_WriteBuffer = NULL;
        m_WorkThread = NULL;
    }
};



struct ThreadNode
{
    pthread_t                            m_ThreadID;
    int                                  m_NotifyRecvPipeFD;
    int                                  m_NotifySendPipeFD;

    struct event_base                   *m_EventBase;
    struct event                         m_NotifyEvent;
    struct MultiEventBaseServer         *m_MultiEventBaseServer;

    std::vector<ConnectNode>             m_ConnectNodeVector;

    ThreadNode()
    {
        m_ThreadID = 0;
        m_NotifyRecvPipeFD = -1;
        m_NotifySendPipeFD = -1;
        m_EventBase = NULL;
        m_MultiEventBaseServer = NULL;
    }
};

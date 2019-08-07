#pragma once
#include <string>
#include <memory>
#include <vector>
#include <event.h>
#include <pthread.h>
#include <event2/http.h>
#include <event2/listener.h>


struct ThreadNode;
const int NETWORK_BUFFER_SIZE = 2048;



class EventBase
{
public:

    EventBase() { m_eventbase = event_base_new(); }
    ~EventBase() { event_base_free(m_eventbase); m_eventbase = NULL; }

    event_base *m_eventbase;
};

class EventNormal
{
public:

    EventNormal() { m_event = NULL; }
    ~EventNormal()
    {
        if (m_event != NULL)
        {
            event_free(m_event);
            m_event = NULL;
        }
    }

    event *m_event;
};

class EventDataBuffer
{
public:

    EventDataBuffer() { m_evbuffer = evbuffer_new(); }
    ~EventDataBuffer() { evbuffer_free(m_evbuffer); m_evbuffer = NULL; }

    evbuffer *m_evbuffer;
};

class EventIOBuffer
{
public:

    EventIOBuffer() { m_bufferevent = NULL; }
    ~EventIOBuffer()
    {
        if (m_bufferevent != NULL)
        {
            bufferevent_free(m_bufferevent);
            m_bufferevent = NULL;
        }
    }

    bufferevent *m_bufferevent;
};

class EventListener
{
public:

    EventListener() { m_listener = NULL; }
    ~EventListener()
    {
        if (m_listener != NULL)
        {
            evconnlistener_free(m_listener);
            m_listener = NULL;
        }
    }

    evconnlistener *m_listener;
};

class EventHTTP
{
public:

    EventHTTP() { m_evhttp = NULL; }
    ~EventHTTP()
    {
        if (m_evhttp != NULL)
        {
            evhttp_free(m_evhttp);
            m_evhttp = NULL;
        }
    }

    evhttp *m_evhttp;
};



struct NetworkMessage
{
    int                                      m_Socket;
    std::string                              m_Message;
    std::shared_ptr<EventIOBuffer>           m_IOBuffer;

    NetworkMessage() : m_Socket(0) {}
};

struct ClientNode
{
    int                                      m_Socket;
    std::shared_ptr<EventIOBuffer>           m_IOBuffer;
    std::shared_ptr<EventDataBuffer>         m_DataBuffer;

    ClientNode() : m_Socket(0), m_IOBuffer(std::make_shared<EventIOBuffer>()), m_DataBuffer(std::make_shared<EventDataBuffer>()) {}
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

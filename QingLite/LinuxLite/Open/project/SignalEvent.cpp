#include "SignalEvent.h"



SignalEvent::SignalEvent()
{
    m_EventBase = NULL;
}

SignalEvent::~SignalEvent()
{
    while (!m_SignalEventMap.empty())
    {
        event_del(m_SignalEventMap.begin()->second);
        m_SignalEventMap.erase(m_SignalEventMap.begin());
    }
}

bool SignalEvent::BindBaseEvent(event_base * EventBase)
{
    m_EventBase = EventBase;
    return m_EventBase != NULL;
}

bool SignalEvent::AddSignalEvent(int Signal, void(*CallBack_Signal)(int, short, void *))
{
    if (m_EventBase == NULL)
    {
        printf("Add signal event failed, base event is NULL.\n");
        return false;
    }

    event *SignalEvent = evsignal_new(m_EventBase, Signal, CallBack_Signal, (void*)this);
    if (SignalEvent == NULL)
    {
        printf("Create signal event error.\n");
        return false;
    }

    if (event_add(SignalEvent, NULL) < 0)
    {
        printf("Add signal event to base error.\n");
        event_del(SignalEvent);
        return false;
    }

    DeleteSignalEvent(Signal);
    m_SignalEventMap[Signal] = SignalEvent;
    return true;
}

bool SignalEvent::DeleteSignalEvent(int Signal)
{
    std::map<int, event*>::iterator iter = m_SignalEventMap.find(Signal);
    if (iter != m_SignalEventMap.end())
    {
        event_del(iter->second);
        m_SignalEventMap.erase(iter);
        return true;
    }

    return false;
}

#include "SignalEventMap.h"
#include "../tools/BoostLog.h"



SignalEventMap::SignalEventMap()
{
    m_EventBase = NULL;
}

SignalEventMap::~SignalEventMap()
{
    while (!m_SignalEventMap.empty())
    {
        event_del(m_SignalEventMap.begin()->second);
        m_SignalEventMap.erase(m_SignalEventMap.begin());
    }
}

bool SignalEventMap::BindBaseEvent(event_base * EventBase)
{
    m_EventBase = EventBase;
    return m_EventBase != NULL;
}

bool SignalEventMap::AddSignalEvent(int Signal, void(*CallBack_Signal)(int, short, void *))
{
    if (m_EventBase == NULL)
    {
        g_Log.WriteError("Add signal event failed, base event is NULL.");
        return false;
    }

    event *SignalEventMap = evsignal_new(m_EventBase, Signal, CallBack_Signal, (void*)this);
    if (SignalEventMap == NULL)
    {
        g_Log.WriteError("Create signal event error.");
        return false;
    }

    if (event_add(SignalEventMap, NULL) < 0)
    {
        g_Log.WriteError("Add signal event to base event error.");
        event_del(SignalEventMap);
        return false;
    }

    DeleteSignalEvent(Signal);
    m_SignalEventMap[Signal] = SignalEventMap;
    return true;
}

bool SignalEventMap::DeleteSignalEvent(int Signal)
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

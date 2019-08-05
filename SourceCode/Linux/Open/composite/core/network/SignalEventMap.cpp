#include "SignalEventMap.h"
#include "../../../../../Common/Boost/BoostLog.h"



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

    g_Log.WriteDebug(BoostFormat("Signal event map was destructored, signal count = %d.", m_SignalEventMap.size()));
}

bool SignalEventMap::BindBaseEvent(event_base *EventBase)
{
    m_EventBase = EventBase;
    return m_EventBase != NULL;
}

bool SignalEventMap::AddSignalEvent(int Signal, void(*CallBack_Signal)(int, short, void *), void *UserData)
{
    if (m_EventBase == NULL)
    {
        g_Log.WriteError("Add signal event failed, base event is NULL.");
        return false;
    }

    event *SignalEventMap = evsignal_new(m_EventBase, Signal, CallBack_Signal, UserData);
    if (SignalEventMap == NULL)
    {
        g_Log.WriteError(BoostFormat("Create signal = %d event error.", Signal));
        return false;
    }

    if (event_add(SignalEventMap, NULL) < 0)
    {
        g_Log.WriteError(BoostFormat("Add signal = %d event to base event error.", Signal));
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
        g_Log.WriteDebug(BoostFormat("Signal map delete signal = %d", Signal));
        return true;
    }

    return false;
}

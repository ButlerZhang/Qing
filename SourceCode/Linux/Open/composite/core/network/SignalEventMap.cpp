#include "SignalEventMap.h"
#include "../../../../../Common/Boost/BoostLog.h"



SignalEventMap::SignalEventMap()
{
}

SignalEventMap::~SignalEventMap()
{
    while (!m_SignalEventMap.empty())
    {
        event_del(m_SignalEventMap.begin()->second.m_event);
        m_SignalEventMap.erase(m_SignalEventMap.begin());
    }

    g_Log.WriteDebug(BoostFormat("Signal event map was destructored, signal count = %d.", m_SignalEventMap.size()));
}


bool SignalEventMap::AddSignalEvent(event_base *EventBase, int Signal, void(*CallBack_Signal)(int, short, void *), void *UserData)
{
    if (EventBase == NULL)
    {
        g_Log.WriteError("Add signal event failed, base event is NULL.");
        return false;
    }

    event *SignalEvent = evsignal_new(EventBase, Signal, CallBack_Signal, UserData);
    if (SignalEvent == NULL)
    {
        g_Log.WriteError(BoostFormat("Create signal = %d event error.", Signal));
        return false;
    }

    if (event_add(SignalEvent, NULL) < 0)
    {
        g_Log.WriteError(BoostFormat("Add signal = %d event to base event error.", Signal));
        event_del(SignalEvent);
        return false;
    }

    DeleteSignalEvent(Signal);
    m_SignalEventMap[Signal].m_event = SignalEvent;
    return true;
}

bool SignalEventMap::DeleteSignalEvent(int Signal)
{
    std::map<int, EventNormal>::iterator iter = m_SignalEventMap.find(Signal);
    if (iter != m_SignalEventMap.end())
    {
        event_del(iter->second.m_event);
        m_SignalEventMap.erase(iter);
        g_Log.WriteDebug(BoostFormat("Signal map delete signal = %d", Signal));
        return true;
    }

    return false;
}

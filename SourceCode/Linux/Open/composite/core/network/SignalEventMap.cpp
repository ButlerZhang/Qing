#include "SignalEventMap.h"
#include "../../../../../Common/Boost/BoostLog.h"



SignalEventMap::SignalEventMap()
{
}

SignalEventMap::~SignalEventMap()
{
    m_SignalMap.clear();
    g_Log.WriteDebug(BoostFormat("Signal event map was destructored, signal count = %d.", m_SignalMap.size()));
}

bool SignalEventMap::AddSignalEvent(event_base *EventBase, int Signal, void(*CallBack_Signal)(int, short, void *), void *UserData)
{
    if (EventBase == NULL)
    {
        g_Log.WriteError("Add signal event failed, base event is NULL.");
        return false;
    }

    EventNormal SignalEvent;
    SignalEvent.m_event = evsignal_new(EventBase, Signal, CallBack_Signal, UserData);
    if (SignalEvent.m_event == NULL)
    {
        g_Log.WriteError(BoostFormat("Create signal = %d event error.", Signal));
        return false;
    }

    if (event_add(SignalEvent.m_event, NULL) < 0)
    {
        g_Log.WriteError(BoostFormat("Add signal = %d event to base event error.", Signal));
        return false;
    }

    DeleteSignalEvent(Signal);
    m_SignalMap[Signal].m_event = SignalEvent.m_event;
    SignalEvent.m_event = NULL;
    return true;
}

bool SignalEventMap::DeleteSignalEvent(int Signal)
{
    std::map<int, EventNormal>::iterator iter = m_SignalMap.find(Signal);
    if (iter != m_SignalMap.end())
    {
        m_SignalMap.erase(iter);
        g_Log.WriteDebug(BoostFormat("Signal map delete signal = %d", Signal));
        return true;
    }

    return false;
}

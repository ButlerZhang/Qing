#pragma once
#include <map>
#include <event.h>



class SignalEvent
{
public:

    SignalEvent();
    ~SignalEvent();

    bool BindBaseEvent(event_base *EventBase);
    bool AddSignalEvent(int Signal, void(*CallBack_Signal)(int, short, void*));
    bool DeleteSignalEvent(int Signal);

private:

    struct event_base          *m_EventBase;
    std::map<int, event*>       m_SignalEventMap;
};
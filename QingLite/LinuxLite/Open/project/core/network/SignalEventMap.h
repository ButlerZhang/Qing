#pragma once
#include <map>
#include <event.h>



class SignalEventMap
{
public:

    SignalEventMap();
    ~SignalEventMap();

    bool BindBaseEvent(event_base *EventBase);
    bool AddSignalEvent(int Signal, void(*CallBack_Signal)(int, short, void*), void *UserData);
    bool DeleteSignalEvent(int Signal);

private:

    struct event_base          *m_EventBase;
    std::map<int, event*>       m_SignalEventMap;
};

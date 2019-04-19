#include "TimerEvent.h"



TimerEvent::TimerEvent()
{
    m_EventBase = NULL;
}

TimerEvent::~TimerEvent()
{
}

bool TimerEvent::BindBaseEvent(event_base * EventBase)
{
    m_EventBase = EventBase;
    return m_EventBase != NULL;
}

event* TimerEvent::AddTimerEvent(void(*CallBack_Timer)(int, short, void*), timeval tv, bool once)
{
    short flag = once ? 0 : EV_PERSIST;

    event *TimerEvent = new event();
    event_assign(TimerEvent, m_EventBase, -1, flag, CallBack_Timer, (void*)this);

    if (event_add(TimerEvent, &tv) < 0)
    {
        event_del(TimerEvent);
        return NULL;
    }

    return TimerEvent;
}

bool TimerEvent::DeleteTimerEvent(event * TimerEvent)
{
    return event_del(TimerEvent) == 0;
}

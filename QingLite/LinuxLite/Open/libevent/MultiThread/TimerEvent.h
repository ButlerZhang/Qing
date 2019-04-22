#pragma once
#include <event.h>



class TimerEvent
{
public:

    TimerEvent();
    ~TimerEvent();

    bool BindBaseEvent(event_base *EventBase);
    event* AddTimerEvent(void(*CallBack_Timer)(int, short, void*), timeval tv, bool once);
    bool DeleteTimerEvent(event *TimerEvent);

private:

    struct event_base          *m_EventBase;
};

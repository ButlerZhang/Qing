#pragma once
#include "NetworkBase.h"
#include <map>



class SignalEventMap
{
public:

    SignalEventMap();
    ~SignalEventMap();

    bool AddSignalEvent(event_base *EventBase, int Signal, void(*CallBack_Signal)(int, short, void*), void *UserData);
    bool DeleteSignalEvent(int Signal);

private:

    std::map<int, EventNormal>                m_SignalEventMap;
};

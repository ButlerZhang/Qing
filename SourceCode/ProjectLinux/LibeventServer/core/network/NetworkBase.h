#pragma once
#include "SignalEventMap.h"



class NetworkBase
{
public:

    NetworkBase();
    virtual ~NetworkBase();

protected:

    virtual bool ProcessCheckout() = 0;
    virtual bool ProcessSignal(int Signal);
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg) = 0;

    bool AddSignal();
    bool AddCheckoutTimer(int TimerInternal);
    void InitializeSocketAddress(struct sockaddr_in &SocketAddress, const std::string &IP, int Port) const;
    void RecvData(int Socket, const std::shared_ptr<EventIOBuffer> &IOBuffer,const std::shared_ptr<EventDataBuffer> &DataBuffer);

    static void CallBack_Checkout(int Socket, short Events, void *UserData);
    static void CallBack_Signal(int Signal, short Events, void *UserData);

protected:

    EventBase                                    m_EventBase;
    EventNormal                                  m_CheckoutTimer;
    SignalEventMap                               m_SignalEventMap;
};

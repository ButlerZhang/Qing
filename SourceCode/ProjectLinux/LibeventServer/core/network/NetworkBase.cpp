#include "NetworkBase.h"
#include "../../../../Share/Boost/BoostLog.h"
#include <arpa/inet.h>
#include <signal.h>



NetworkBase::NetworkBase()
{
}

NetworkBase::~NetworkBase()
{
}

bool NetworkBase::ProcessSignal(int Signal)
{
    if (Signal == SIGINT || Signal == SIGTERM)
    {
        g_Log.WriteDebug(BoostFormat("Network base captured signal = %d and prepared to quit.", Signal));
        g_Log.Flush();

        event_base_loopexit(m_EventBase.m_eventbase, NULL);
        return true;
    }

    return false;
}

bool NetworkBase::AddSignal()
{
    std::vector<int> SignalVector = { SIGINT, SIGTERM };
    for (std::vector<int>::const_iterator it = SignalVector.begin(); it != SignalVector.end(); it++)
    {
        if (!m_SignalEventMap.AddSignalEvent(m_EventBase.m_eventbase, *it, CallBack_Signal, this))
        {
            g_Log.WriteError(BoostFormat("Network base add signal = %d failed.", *it));
            return false;
        }
    }

    return true;
}

bool NetworkBase::AddCheckoutTimer(int TimerInternal)
{
    if (m_CheckoutTimer.m_event != NULL)
    {
        g_Log.WriteError("Network base re-create checkout timer.");
        return true;
    }

    m_CheckoutTimer.m_event = event_new(m_EventBase.m_eventbase, -1, EV_PERSIST, CallBack_Checkout, this);
    if (m_CheckoutTimer.m_event == NULL)
    {
        g_Log.WriteError("Network base create chekcout timer failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = TimerInternal;
    if (event_add(m_CheckoutTimer.m_event, &tv) != 0)
    {
        g_Log.WriteError("Network base add checkout timer failed.");
        return false;
    }

    return true;
}

void NetworkBase::InitializeSocketAddress(sockaddr_in &SocketAddress, const std::string &IP, int Port) const
{
    bzero(&SocketAddress, sizeof(sockaddr_in));

    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_port = htons(static_cast<uint16_t>(Port));

    if (!IP.empty())
    {
        inet_pton(AF_INET, IP.c_str(), &(SocketAddress.sin_addr));
    }
    else
    {
        SocketAddress.sin_addr.s_addr = INADDR_ANY;
    }
}

void NetworkBase::RecvData(int Socket, const std::shared_ptr<EventIOBuffer> &IOBuffer, const std::shared_ptr<EventDataBuffer> &DataBuffer)
{
    const size_t MESSAGE_HEADER_LENGTH_SIZE = 4;
    std::vector<char> MessageHeaderLengthBuffer(MESSAGE_HEADER_LENGTH_SIZE, 0);

    static unsigned long long RecvCount = 0;
    std::vector<char> RecvBuffer(NETWORK_BUFFER_SIZE, 0);

    size_t RecvSize = 0, EventBufferLength = 0;
    while (RecvSize = bufferevent_read(IOBuffer->m_bufferevent, &RecvBuffer[0], RecvBuffer.size()), RecvSize > 0)
    {
        g_Log.WriteDebug(BoostFormat("Network base recv socket = %d message size = %llu", Socket, RecvSize));

        if (evbuffer_add(DataBuffer->m_evbuffer, &RecvBuffer[0], RecvSize) != 0)
        {
            g_Log.WriteError(BoostFormat("Network base recv message can not add to evbuffer, Socket = %d.", Socket));
            continue;
        }

        while (EventBufferLength = evbuffer_get_length(DataBuffer->m_evbuffer), EventBufferLength > 0)
        {
            if (EventBufferLength <= MESSAGE_HEADER_LENGTH_SIZE)
            {
                g_Log.WriteError(BoostFormat("Network base recv message length is only %llu bytes.", EventBufferLength));
                break;
            }

            if (evbuffer_copyout(DataBuffer->m_evbuffer, &MessageHeaderLengthBuffer[0], MESSAGE_HEADER_LENGTH_SIZE) != MESSAGE_HEADER_LENGTH_SIZE)
            {
                g_Log.WriteError("Network base can not copy out message length.");
                break;
            }

            int MessageBodyLength = 0;
            ::memcpy(&MessageBodyLength, &MessageHeaderLengthBuffer[0], sizeof(int));
            size_t MessageTotalLength = ::ntohl(MessageBodyLength) + MESSAGE_HEADER_LENGTH_SIZE;

            g_Log.WriteDebug(BoostFormat("Network base recv total length = %llu, event buffer length = %llu.", MessageTotalLength, EventBufferLength));
            if (EventBufferLength < MessageTotalLength)
            {
                g_Log.WriteError("Network base, event buffer length is less than message total length, break while.");
                break;
            }

            if (MessageTotalLength > RecvBuffer.size())
            {
                g_Log.WriteDebug(BoostFormat("Network base recv buffer old size = %llu, new size = %llu.", RecvBuffer.size(), MessageTotalLength));
                RecvBuffer.resize(MessageTotalLength, 0);
            }

            if (evbuffer_remove(DataBuffer->m_evbuffer, &RecvBuffer[0], MessageTotalLength) != static_cast<int>(MessageTotalLength))
            {
                g_Log.WriteError(BoostFormat("Network base can not remove message size = %llu.", MessageTotalLength));
                break;
            }

            g_Log.WriteInfo(BoostFormat("Network base recv size = %d, recv count = %llu", MessageTotalLength, ++RecvCount));

            NetworkMessage NetworkMsg;
            NetworkMsg.m_Socket = Socket;
            NetworkMsg.m_IOBuffer = IOBuffer;
            NetworkMsg.m_Message.assign(RecvBuffer.begin(), RecvBuffer.begin() + MessageTotalLength);
            ProcessMessage(NetworkMsg);
        }
    }
}

void NetworkBase::CallBack_Checkout(int Socket, short Events, void *UserData)
{
    NetworkBase *MyNetwork = (NetworkBase*)UserData;
    MyNetwork->ProcessCheckout();

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 5;      //TODO
    event_add(MyNetwork->m_CheckoutTimer.m_event, &tv);
}

void NetworkBase::CallBack_Signal(int Signal, short Events, void *UserData)
{
    NetworkBase *MyNetwork = (NetworkBase*)UserData;
    if (!MyNetwork->ProcessSignal(Signal))
    {
        g_Log.WriteError(BoostFormat("Can not process signal = %d", Signal));
    }
}

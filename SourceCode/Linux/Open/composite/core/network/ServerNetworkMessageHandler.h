#pragma once
#include "NetworkBase.h"
#include <mutex>
#include <queue>
#include <condition_variable>

struct bufferevent;
class SingleEventBaseServer;



class ServerNetworkMessageHandler
{
public:

    ServerNetworkMessageHandler();
    ~ServerNetworkMessageHandler();

    bool Start(SingleEventBaseServer *SingleServer);
    bool Stop();

    bool PushMessage(const NetworkMessage &NetworkMsg);

private:

    static void WorkThread_Process(void *Object);

private:

    bool                                 m_IsWork;
    std::mutex                           m_QueueLock;
    std::condition_variable              m_Condition;
    std::queue<NetworkMessage>           m_NetworkMsgQueue;
    SingleEventBaseServer               *m_SingleServer;
};

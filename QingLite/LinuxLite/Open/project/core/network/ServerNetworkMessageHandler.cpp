#include "ServerNetworkMessageHandler.h"
#include "SingleEventBaseServer.h"
#include "../tools/BoostLog.h"
#include <thread>
#include <unistd.h>
#include <sstream>
#include <event2/bufferevent.h>



ServerNetworkMessageHandler::ServerNetworkMessageHandler()
{
    m_IsWork = false;
    m_SingleServer = NULL;
}

ServerNetworkMessageHandler::~ServerNetworkMessageHandler()
{
    m_IsWork = false;
    m_SingleServer = NULL;
    while (!m_NetworkMsgQueue.empty())
    {
        m_NetworkMsgQueue.pop();
    }
}

bool ServerNetworkMessageHandler::Start(SingleEventBaseServer *SingleServer)
{
    if (SingleServer == NULL)
    {
        g_Log.WriteError("Start message handler faile because single server is NULL.");
        return false;
    }

    if (!m_IsWork)
    {
        m_IsWork = true;
        m_SingleServer = SingleServer;

        std::thread Worker(WorkThread_Process, this);
        Worker.detach();
    }

    return m_IsWork;
}

bool ServerNetworkMessageHandler::Stop()
{
    m_IsWork = false;
    m_SingleServer = NULL;
    return true;
}

bool ServerNetworkMessageHandler::PushMessage(const NetworkMessage &NetworkMsg)
{
    std::unique_lock<std::mutex> Locker(m_QueueLock);
    m_NetworkMsgQueue.push(NetworkMsg);

    g_Log.WriteDebug(BoostFormat("Message queue size = %d.", m_NetworkMsgQueue.size()));
    m_Condition.notify_one();

    return !m_NetworkMsgQueue.empty();
}

void ServerNetworkMessageHandler::WorkThread_Process(void *Object)
{
    ServerNetworkMessageHandler *Handler = (ServerNetworkMessageHandler*)Object;

    std::stringstream stream;
    stream << std::this_thread::get_id();
    unsigned long long ThreadID = std::stoull(stream.str());
    g_Log.WriteDebug(BoostFormat("Network message handler thread = %u start...", ThreadID));

    while (Handler->m_IsWork)
    {
        if (Handler->m_NetworkMsgQueue.empty())
        {
            std::unique_lock<std::mutex> Locker(Handler->m_QueueLock);
            Handler->m_Condition.wait(Locker);
        }

        if (!Handler->m_NetworkMsgQueue.empty())
        {
            std::unique_lock<std::mutex> Locker(Handler->m_QueueLock);

            //my work is sleep.
            //std::this_thread::sleep_for(std::chrono::milliseconds(500));

            Handler->m_SingleServer->ProcessMessage(Handler->m_NetworkMsgQueue.front());
            Handler->m_NetworkMsgQueue.pop();
        }
    }

    g_Log.WriteInfo(BoostFormat("Wrok thread = %u stop.", ThreadID));
}

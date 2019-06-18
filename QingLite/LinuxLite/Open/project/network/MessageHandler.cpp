#include "MessageHandler.h"
#include "SingleEventBaseServer.h"

#include <thread>
#include <unistd.h>
#include <sstream>
#include <event2/bufferevent.h>



MessageHandler::MessageHandler()
{
    m_IsWork = false;
    m_SingleServer = NULL;
}

MessageHandler::~MessageHandler()
{
    m_IsWork = false;
    m_SingleServer = NULL;
    while (!m_MessageQueue.empty())
    {
        m_MessageQueue.pop();
    }
}

bool MessageHandler::Start(SingleEventBaseServer *SingleServer)
{
    if (SingleServer == NULL)
    {
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

bool MessageHandler::Stop()
{
    m_IsWork = false;
    m_SingleServer = NULL;
    return true;
}

bool MessageHandler::PushMessage(int ClientSocket, bufferevent *bev, const std::string &Message)
{
    MessageNode NewNode;
    NewNode.m_Message = Message;
    NewNode.m_bufferevent = bev;
    NewNode.m_ClientSocket = ClientSocket;

    std::unique_lock<std::mutex> Locker(m_QueueLock);
    m_MessageQueue.push(NewNode);

    printf("Message queue size = %d.\n", m_MessageQueue.size());
    m_Condition.notify_one();

    return !m_MessageQueue.empty();
}

void MessageHandler::WorkThread_Process(void *Object)
{
    MessageHandler *Handler = (MessageHandler*)Object;

    std::stringstream stream;
    stream << std::this_thread::get_id();
    unsigned long long ThreadID = std::stoull(stream.str());
    printf("Wrok thread = %u start...\n", ThreadID);

    while (Handler->m_IsWork)
    {
        if (Handler->m_MessageQueue.empty())
        {
            std::unique_lock<std::mutex> Locker(Handler->m_QueueLock);
            Handler->m_Condition.wait(Locker);
        }

        if (!Handler->m_MessageQueue.empty())
        {
            std::unique_lock<std::mutex> Locker(Handler->m_QueueLock);

            //my work is sleep.
            //std::this_thread::sleep_for(std::chrono::milliseconds(500));

            Handler->m_SingleServer->ProcessMessage(Handler->m_MessageQueue.front());
            Handler->m_MessageQueue.pop();
        }
    }

    printf("Wrok thread = %u stop.\n", ThreadID);
}

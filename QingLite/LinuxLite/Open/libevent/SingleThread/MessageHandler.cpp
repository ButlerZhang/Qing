#include "MessageHandler.h"
#include <thread>
#include <unistd.h>
#include <event2/bufferevent.h>



MessageHandler::MessageHandler()
{
    m_IsWork = false;
}

MessageHandler::~MessageHandler()
{
    m_IsWork = false;
    while (!m_MessageQueue.empty())
    {
        m_MessageQueue.pop();
    }
}

bool MessageHandler::Start()
{
    if (!m_IsWork)
    {
        m_IsWork = true;

        std::thread Worker(WorkThread, this);
        Worker.detach();
    }

    return m_IsWork;
}

bool MessageHandler::Stop()
{
    m_IsWork = false;
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
    m_Condition.notify_one();

    if (m_MessageQueue.size() > 1)
    {
        printf("Message queue size = %d.\n", m_MessageQueue.size());
    }

    return !m_MessageQueue.empty();
}

void MessageHandler::WorkThread(void *Object)
{
    MessageHandler *Handler = (MessageHandler*)Object;
    while (Handler->m_IsWork)
    {
        if (Handler->m_MessageQueue.empty())
        {
            std::unique_lock<std::mutex> Locker(Handler->m_QueueLock);
            Handler->m_Condition.wait(Locker);
        }

        std::unique_lock<std::mutex> Locker(Handler->m_QueueLock);
        const MessageNode &Node = Handler->m_MessageQueue.front();

        //my work is sleep.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        const std::string ACK("ACK");
        bufferevent_write(Node.m_bufferevent, ACK.c_str(), ACK.length());
        printf("Client = %d send ack, size = %d.\n", Node.m_ClientSocket, ACK.length());
        Handler->m_MessageQueue.pop();
    }

    printf("Work thread stop.\n");
}

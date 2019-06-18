#pragma once
#include <mutex>
#include <queue>
#include <string>
#include <condition_variable>

class SingleEventBaseServer;



class MessageHandler
{
public:

    struct MessageNode
    {
        int                              m_ClientSocket;
        std::string                      m_Message;
        struct bufferevent              *m_bufferevent;
    };

public:

    MessageHandler();
    ~MessageHandler();

    bool Start(SingleEventBaseServer *SingleServer);
    bool Stop();

    bool PushMessage(int ClientSocket, bufferevent *bev, const std::string &Message);

private:

    static void WorkThread_Process(void *Object);

private:

    bool                                 m_IsWork;
    std::mutex                           m_QueueLock;
    std::condition_variable              m_Condition;
    std::queue<MessageNode>              m_MessageQueue;
    SingleEventBaseServer               *m_SingleServer;
};

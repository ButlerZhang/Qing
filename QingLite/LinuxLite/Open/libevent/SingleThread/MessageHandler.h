#pragma once
#include <mutex>
#include <queue>
#include <string>
#include <condition_variable>



struct MessageNode
{
    int                     m_ClientSocket;
    std::string             m_Message;
    struct bufferevent     *m_bufferevent;
};



class MessageHandler
{
public:

    MessageHandler();
    ~MessageHandler();

    bool Start();
    bool Stop();

    bool PushMessage(int ClientSocket, bufferevent *bev, const std::string &Message);

private:

    static void WorkThread(void *Object);

private:

    bool                            m_IsWork;
    std::mutex                      m_QueueLock;
    std::condition_variable         m_Condition;
    std::queue<MessageNode>         m_MessageQueue;
};

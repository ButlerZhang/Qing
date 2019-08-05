#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>



class ThreadNoticeQueue
{
public:

    static ThreadNoticeQueue& GetInstance();

    bool PopMessage(std::string &JsonString);
    bool PushMessage(const std::string &JsonString);

    int  GetRecvDescriptor() const { return m_RecvDescriptor; }
    int  GetSendDescriptor() const { return m_SendDescriptor; }

private:

    ThreadNoticeQueue();
    ~ThreadNoticeQueue();

private:

    int                                  m_RecvDescriptor;
    int                                  m_SendDescriptor;
    std::mutex                           m_QueueLock;
    std::queue<std::string>              m_Queue;
};

#define g_ThreadNoticeQueue ThreadNoticeQueue::GetInstance()

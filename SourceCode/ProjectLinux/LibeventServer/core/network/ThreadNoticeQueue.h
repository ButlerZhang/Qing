#pragma once
#include "LibeventInterface.h"
#include <mutex>
#include <queue>
#include <condition_variable>



class ThreadNoticeQueue
{
public:

    bool PopMessage(NetworkMessage &NetworkMsg);
    bool PushMessage(int NoticeType, const std::string &JsonString);

    int  GetRecvDescriptor() const { return m_RecvDescriptor; }
    int  GetSendDescriptor() const { return m_SendDescriptor; }

    static ThreadNoticeQueue& GetInstance() { static ThreadNoticeQueue g_Instance; return g_Instance; }

private:

    ThreadNoticeQueue();
    ~ThreadNoticeQueue();

private:

    int                                  m_RecvDescriptor;
    int                                  m_SendDescriptor;
    std::mutex                           m_QueueLock;
    std::queue<NetworkMessage>           m_Queue;
};

#define g_ThreadNoticeQueue ThreadNoticeQueue::GetInstance()

#include "ThreadNoticeQueue.h"
#include "../../../../../Common/Boost/BoostLog.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>



ThreadNoticeQueue& ThreadNoticeQueue::GetInstance()
{
    static ThreadNoticeQueue m_ThreadNoticeQueue;
    return m_ThreadNoticeQueue;
}

ThreadNoticeQueue::ThreadNoticeQueue()
{
    int ThreadPipe[2];
    ThreadPipe[0] = -1;
    ThreadPipe[1] = -1;

    pipe(ThreadPipe);
    m_RecvDescriptor = ThreadPipe[0];
    m_SendDescriptor = ThreadPipe[1];
    g_Log.WriteDebug(BoostFormat("Thread notice queue create pipe, recv = %d, send = %d.", m_RecvDescriptor, m_SendDescriptor));
}

ThreadNoticeQueue::~ThreadNoticeQueue()
{
    while(!m_Queue.empty())
    {
        m_Queue.pop();
    }

    g_Log.WriteDebug(BoostFormat("Thread notice queue was destructored, size = %d.", m_Queue.size()));
}

bool ThreadNoticeQueue::PopMessage(std::string &JsonString)
{
    std::unique_lock<std::mutex> Locker(m_QueueLock);

    int NoticeType = 1; //TODO
    read(m_RecvDescriptor, &NoticeType, sizeof(NoticeType));

    if (m_Queue.empty())
    {
        g_Log.WriteError("Thread notice queue no data, can not pop.");
        return false;
    }

    JsonString = m_Queue.front();
    m_Queue.pop();
    return true;
}

bool ThreadNoticeQueue::PushMessage(const std::string &JsonString)
{
    std::unique_lock<std::mutex> Locker(m_QueueLock);
    if (m_RecvDescriptor < 0 || m_SendDescriptor < 0)
    {
        g_Log.WriteError(BoostFormat("Thread notice queue descriptor is wrong, recv = %d, send = %d.", m_RecvDescriptor, m_SendDescriptor));
        return false;
    }

    m_Queue.push(JsonString);
    g_Log.WriteDebug(BoostFormat("Thread notice queue size = %d", m_Queue.size()));

    int NoticeType = 1; //TODO
    if (write(m_SendDescriptor, &NoticeType, sizeof(NoticeType)) != sizeof(int))
    {
        g_Log.WriteError("Thread notice queue write descriptor wrong.");
        return false;
    }

    return true;
}

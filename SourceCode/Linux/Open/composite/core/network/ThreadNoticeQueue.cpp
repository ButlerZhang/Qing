#include "ThreadNoticeQueue.h"
#include "../../../../../Common/Boost/BoostLog.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>



ThreadNoticeQueue::ThreadNoticeQueue()
{
    int ThreadPipe[2];
    ThreadPipe[0] = -1;
    ThreadPipe[1] = -1;

    if (pipe(ThreadPipe) == 0)
    {
        m_RecvDescriptor = ThreadPipe[0];
        m_SendDescriptor = ThreadPipe[1];
    }
    else
    {
        m_RecvDescriptor = m_SendDescriptor = -1;
        g_Log.WriteError(BoostFormat("Thread notice queue create pipe error = %s.", strerror(errno)));
    }
}

ThreadNoticeQueue::~ThreadNoticeQueue()
{
    while (!m_Queue.empty())
    {
        m_Queue.pop();
    }

    g_Log.WriteDebug(BoostFormat("Thread notice queue was destructored, size = %d.", m_Queue.size()));
}

bool ThreadNoticeQueue::PopMessage(std::string &JsonString)
{
    std::unique_lock<std::mutex> Locker(m_QueueLock);

    int NoticeType = 1;
    if (read(m_RecvDescriptor, &NoticeType, sizeof(NoticeType)) != sizeof(NoticeType))
    {
        //TODO
        //Now, the if statement is added to avoid compile warnings.
    }

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
    if (m_RecvDescriptor <= 0 || m_SendDescriptor <= 0)
    {
        g_Log.WriteError(BoostFormat("Thread notice queue descriptors are wrong, recv = %d, send = %d.", m_RecvDescriptor, m_SendDescriptor));
        return false;
    }

    m_Queue.push(JsonString);
    g_Log.WriteDebug(BoostFormat("Thread notice queue push message, current size = %d", m_Queue.size()));

    int NoticeType = 1; //TODO
    if (write(m_SendDescriptor, &NoticeType, sizeof(NoticeType)) != sizeof(int))
    {
        g_Log.WriteError("Thread notice queue write descriptor wrong.");
        return false;
    }

    return true;
}

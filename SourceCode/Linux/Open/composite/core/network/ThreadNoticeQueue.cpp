#include "ThreadNoticeQueue.h"
#include "../../../../../BoostLinux/src/BoostLog.h"
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

    g_Log.WriteDebug(BoostFormat("Thread notice queue was destructored, queue size = %d.", m_Queue.size()));
}

bool ThreadNoticeQueue::PopMessage(NetworkMessage &NetworkMsg)
{
    std::unique_lock<std::mutex> Locker(m_QueueLock);

    int NoticeType = -1;
    if (read(m_RecvDescriptor, &NoticeType, sizeof(NoticeType)) != sizeof(NoticeType))
    {
        g_Log.WriteError(BoostFormat("Thread notice queue read descriptor wrong, message type = %d.", NoticeType));
        return false;
    }

    if (m_Queue.empty())
    {
        g_Log.WriteError(BoostFormat("Thread notice queue no data, can not pop, message type = %d.", NoticeType));
        return false;
    }

    NetworkMsg = m_Queue.front();
    m_Queue.pop();

    g_Log.WriteDebug(BoostFormat("Thread notice queue pop type = %d, read type = %d", NetworkMsg.m_MessageType, NoticeType));
    return true;
}

bool ThreadNoticeQueue::PushMessage(int NoticeType, const std::string &JsonString)
{
    std::unique_lock<std::mutex> Locker(m_QueueLock);
    if (m_RecvDescriptor <= 0 || m_SendDescriptor <= 0)
    {
        g_Log.WriteError(BoostFormat("Thread notice queue descriptors are wrong, recv = %d, send = %d.", m_RecvDescriptor, m_SendDescriptor));
        return false;
    }

    NetworkMessage NewMsg;
    NewMsg.m_Message = JsonString;
    NewMsg.m_MessageType = NoticeType;

    m_Queue.push(NewMsg);
    g_Log.WriteDebug(BoostFormat("Thread notice queue push message type = %d, queue size = %d", NoticeType, m_Queue.size()));

    if (write(m_SendDescriptor, &NoticeType, sizeof(NoticeType)) != sizeof(int))
    {
        g_Log.WriteError(BoostFormat("Thread notice queue write descriptor wrong, message type = %d.", NoticeType));
        return false;
    }

    return true;
}

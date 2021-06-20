#pragma once
#include "IOCPModel.h"
#include "../../../Share/Boost/BoostLog.h"



void ReleaseSocket(SOCKET &Socket)
{
    if (Socket != INVALID_SOCKET)
    {
        ::shutdown(Socket, SD_BOTH);
        ::closesocket(Socket);

        g_Log.WriteInfo(BoostFormat("Release Socket = %I64d.", Socket));
        Socket = INVALID_SOCKET;
    }
}



IOCPContext::IOCPContext(unsigned __int64 ID)
{
    ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
    ZeroMemory(&m_Buffer, sizeof(m_Buffer));
    m_AcceptSocket = INVALID_SOCKET;
    m_WSABuffer.buf = m_Buffer;
    m_WSABuffer.len = MAX_IO_CONTEXT_BUFFER_LEN;
    m_ActionType = IOCP_AT_NULL;
    m_ContextID = ID;
}

IOCPContext::~IOCPContext()
{
}

void IOCPContext::ResetBuffer()
{
    ZeroMemory(&m_Buffer, sizeof(m_Buffer));
}



IOCPSocketContext::IOCPSocketContext()
{
    m_Socket = INVALID_SOCKET;
    InitializeCriticalSection(&m_QueueSection);
    ZeroMemory(&m_RemoteAddr, sizeof(m_RemoteAddr));
}

IOCPSocketContext::~IOCPSocketContext()
{
    DeleteCriticalSection(&m_QueueSection);
}

std::shared_ptr<IOCPContext> IOCPSocketContext::GetNewIOContext(unsigned __int64 ID)
{
    std::shared_ptr<IOCPContext> NewIOCPContext = std::make_shared<IOCPContext>(ID);

    EnterCriticalSection(&m_QueueSection);
    m_SendIOCPContextQueue.push(NewIOCPContext);
    LeaveCriticalSection(&m_QueueSection);

    g_Log.WriteInfo(BoostFormat("Push new IOCPContext, Socket = %I64d, IOCPContextID = %I64d.",
        NewIOCPContext->m_AcceptSocket, NewIOCPContext->m_ContextID));

    return NewIOCPContext;
}

bool IOCPSocketContext::DeleteContext(const IOCPContext &RemoveIOContext)
{
    bool IsRemove = false;
    SOCKET RemoveSocket = RemoveIOContext.m_AcceptSocket;
    unsigned __int64 RemoveTrackID = RemoveIOContext.m_ContextID;

    EnterCriticalSection(&m_QueueSection);
    if (!m_SendIOCPContextQueue.empty())
    {
        std::shared_ptr<IOCPContext> OldContext = m_SendIOCPContextQueue.front();
        if (RemoveSocket == OldContext->m_AcceptSocket && RemoveTrackID == OldContext->m_ContextID)
        {
            m_SendIOCPContextQueue.pop();
            IsRemove = true;
        }
    }
    LeaveCriticalSection(&m_QueueSection);

    g_Log.WriteInfo(BoostFormat("Delete IOCPContext %s, Socket = %I64d, IOCPContextID = %I64d.",
        IsRemove ? "succeed" : "failed",
        RemoveSocket,
        RemoveTrackID));

    return true;
}

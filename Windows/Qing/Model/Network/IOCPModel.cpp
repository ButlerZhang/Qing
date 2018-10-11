#pragma once
#include "IOCPModel.h"
#include "..\..\HeaderFiles\QingLog.h"

QING_NAMESPACE_BEGIN



void ReleaseSocket(SOCKET &Socket)
{
    if (Socket != INVALID_SOCKET)
    {
        ::shutdown(Socket, SD_BOTH);
        ::closesocket(Socket);

        //QingLog::Write(LL_INFO, "Release socket = %d.", Socket);
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
    ReleaseSocket(m_AcceptSocket);
}

void IOCPContext::ResetBuffer()
{
    ZeroMemory(&m_Buffer, sizeof(m_Buffer));
}



IOCPSocketContext::IOCPSocketContext()
{
    m_Socket = INVALID_SOCKET;
    ZeroMemory(&m_ClientAddr, sizeof(m_ClientAddr));
}

IOCPSocketContext::~IOCPSocketContext()
{
    ReleaseSocket(m_Socket);
}

std::shared_ptr<IOCPContext> IOCPSocketContext::GetNewIOContext(unsigned __int64 ID)
{
    std::shared_ptr<IOCPContext> NewIOContext = std::make_shared<IOCPContext>(ID);
    m_IOContextVector.push_back(NewIOContext);
    return NewIOContext;
}

bool IOCPSocketContext::RemoveContext(const std::shared_ptr<IOCPContext> &RemoveIOContext)
{
    for (auto Index = 0; Index < m_IOContextVector.size(); Index++)
    {
        if (RemoveIOContext == m_IOContextVector[Index])
        {
            m_IOContextVector.erase(m_IOContextVector.begin() + Index);
            break;
        }
    }

    return false;
}

QING_NAMESPACE_END
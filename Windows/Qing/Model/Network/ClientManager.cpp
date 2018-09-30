#include "ClientManager.h"

QING_NAMESPACE_BEGIN



ClientManager::ClientManager()
{
    InitializeCriticalSection(&m_csClientMapSection);
}

ClientManager::~ClientManager()
{
    ClearAllClient();
    DeleteCriticalSection(&m_csClientMapSection);
}

void ClientManager::ClearAllClient()
{
    EnterCriticalSection(&m_csClientMapSection);
    m_ClientMap.clear();
    LeaveCriticalSection(&m_csClientMapSection);
}

void ClientManager::AddClient(const std::shared_ptr<IOCPSocketContext>& pSocketContext)
{
    EnterCriticalSection(&m_csClientMapSection);
    m_ClientMap.insert(std::pair<SOCKET, std::shared_ptr<IOCPSocketContext>>(pSocketContext->m_Socket, pSocketContext));
    LeaveCriticalSection(&m_csClientMapSection);
}

void ClientManager::RemoveClient(const std::shared_ptr<IOCPSocketContext>& pSocketContext)
{
    EnterCriticalSection(&m_csClientMapSection);
    for (auto iter = m_ClientMap.cbegin(); iter != m_ClientMap.cend(); iter++)
    {
        if (pSocketContext->m_Socket == iter->second->m_Socket)
        {
            m_ClientMap.erase(iter);
            break;
        }
    }
    LeaveCriticalSection(&m_csClientMapSection);
}

QING_NAMESPACE_END
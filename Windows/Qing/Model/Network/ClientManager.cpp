#include "ClientManager.h"

QING_NAMESPACE_BEGIN



ClientManager::ClientManager()
{
    InitializeCriticalSection(&m_MapSection);
}

ClientManager::~ClientManager()
{
    ClearAllClient();
    DeleteCriticalSection(&m_MapSection);
}

void ClientManager::ClearAllClient()
{
    EnterCriticalSection(&m_MapSection);
    m_ClientMap.clear();
    LeaveCriticalSection(&m_MapSection);
}

void ClientManager::AddClient(const std::shared_ptr<IOCPSocketContext>& pSocketContext)
{
    EnterCriticalSection(&m_MapSection);
    m_ClientMap.insert(std::pair<SOCKET, std::shared_ptr<IOCPSocketContext>>(pSocketContext->m_Socket, pSocketContext));
    LeaveCriticalSection(&m_MapSection);
}

void ClientManager::RemoveClient(const std::shared_ptr<IOCPSocketContext>& pSocketContext)
{
    EnterCriticalSection(&m_MapSection);
    for (auto iter = m_ClientMap.cbegin(); iter != m_ClientMap.cend(); iter++)
    {
        if (pSocketContext->m_Socket == iter->second->m_Socket)
        {
            ReleaseSocket(pSocketContext->m_Socket);
            m_ClientMap.erase(iter);
            break;
        }
    }
    LeaveCriticalSection(&m_MapSection);
}

std::shared_ptr<IOCPSocketContext> ClientManager::GetClientContext(SOCKET ClientID)
{
    EnterCriticalSection(&m_MapSection);
    const std::shared_ptr<IOCPSocketContext> &TargetClient = m_ClientMap[ClientID];
    LeaveCriticalSection(&m_MapSection);
    return TargetClient;
}

QING_NAMESPACE_END
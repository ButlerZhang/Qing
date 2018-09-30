#pragma once
#include "IOCPModel.h"
#include <map>

QING_NAMESPACE_BEGIN



class ClientManager
{
public:

    ClientManager();
    ~ClientManager();

    void ClearAllClient();
    void AddClient(const std::shared_ptr<IOCPSocketContext> &pSocketContext);
    void RemoveClient(const std::shared_ptr<IOCPSocketContext> &pSocketContext);

private:

    CRITICAL_SECTION                                        m_csClientMapSection;
    std::map<SOCKET, std::shared_ptr<IOCPSocketContext>>    m_ClientMap;
};

QING_NAMESPACE_END
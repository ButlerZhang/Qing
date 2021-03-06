#pragma once
#include "IOCPModel.h"
#include <map>



class ClientManager
{
public:

    ClientManager();
    ~ClientManager();

    void ClearAllClient();
    void AddClient(const std::shared_ptr<IOCPSocketContext> &pSocketContext);
    void RemoveClient(const std::shared_ptr<IOCPSocketContext> &pSocketContext);

    int  GetAllClientID(std::vector<SOCKET> &ClientVector);
    int  GetClientCount() const { return static_cast<int>(m_ClientMap.size()); }

    std::shared_ptr<IOCPSocketContext> GetClientContext(SOCKET ClientID);

private:

    CRITICAL_SECTION                                        m_MapSection;
    std::map<SOCKET, std::shared_ptr<IOCPSocketContext>>    m_ClientMap;
};

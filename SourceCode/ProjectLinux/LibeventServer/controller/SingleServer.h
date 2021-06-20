#pragma once
#include "RestartManager.h"
#include "../core/tools/Ethernet.h"
#include "../core/network/SingleEventBaseServer.h"
#include "../../../Share/Database/MySQLDatabase.h"
#include <google/protobuf/message.h>

class TCPHandler;



class SingleServer : public SingleEventBaseServer
{
public:

    bool Start(long WatchDogPID);

    virtual bool ProcessCheckout();
    virtual bool ProcessConnected();
    virtual bool ProcessDisconnected();
    virtual bool ProcessThreadNoticeQueue();
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg);

    MySQLDatabase& GetDB() { return m_SMIBDB; }
    Ethernet& GetEthernet() { return m_Ethernet; }
    RestartManager& GetRestartManager() { return m_RestartManager; }
    static SingleServer& GetInstance() { static SingleServer g_Instance; return g_Instance; }

    bool SendMessage(int MessageType, const google::protobuf::Message &ProtobufMsg);
    bool SendMessage(int MessageType, NetworkMessage &NetworkMsg, const google::protobuf::Message &ProtobufMsg);

private:

    SingleServer();
    virtual ~SingleServer();
    bool DispatchMessage(NetworkMessage &NetworkMsg);

private:

    Ethernet                                        m_Ethernet;
    MySQLDatabase                                   m_SMIBDB;
    RestartManager                                  m_RestartManager;
    std::vector<std::shared_ptr<TCPHandler>>        m_HandlerVector;
};

#define g_SingleServer SingleServer::GetInstance()

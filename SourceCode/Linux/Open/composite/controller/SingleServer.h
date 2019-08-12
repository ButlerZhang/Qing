#pragma once
#include "../core/network/SingleEventBaseServer.h"
#include "../../../../Common/Database/MySQLDatabase.h"
#include "../handler/tcphandler/TCPHandler.h"
#include <google/protobuf/message.h>



class SingleServer : public SingleEventBaseServer
{
public:

    virtual bool Start(const std::string &IP, int Port);

    virtual bool ProcessCheckout();
    virtual bool ProcessConnected();
    virtual bool ProcessDisconnected();
    virtual bool ProcessThreadNoticeQueue();
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg);

    MySQLDatabase& GetDB() { return m_SMIBDB; }
    static SingleServer& GetInstance() { static SingleServer g_Instance; return g_Instance; }

    bool SendMessage(int MessageType, const google::protobuf::Message &ProtobufMsg);
    bool SendMessage(int MessageType, NetworkMessage &NetworkMsg, const google::protobuf::Message &ProtobufMsg);

private:

    SingleServer();
    virtual ~SingleServer();

private:

    MySQLDatabase                               m_SMIBDB;
    std::vector<std::shared_ptr<TCPHandler>>    m_HandlerVector;
};

#define g_SingleServer SingleServer::GetInstance()

#pragma once
#include "../core/network/SingleEventBaseServer.h"
#include "../../../../Common/Database/MySQLDatabase.h"
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

private:

    SingleServer();
    virtual ~SingleServer();

    bool ProcessLogin(NetworkMessage &NetworkMsg);
    bool ProcessLogout(NetworkMessage &NetworkMsg);

    bool SendMessage(int MessageType, const google::protobuf::Message &ProtobufMsg);
    bool SendMessage(int MessageType, NetworkMessage &NetworkMsg, const google::protobuf::Message &ProtobufMsg);

private:

    MySQLDatabase                       m_SMIBDB;
};

#define g_SingleServer SingleServer::GetInstance()

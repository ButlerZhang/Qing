#pragma once
#include "../core/network/SingleEventBaseServer.h"
#include "../core/database/MySQLDatabase.h"
#include <google/protobuf/message.h>

class HTTPBaseServer;



class SingleServer : public SingleEventBaseServer
{
public:

    SingleServer();
    virtual ~SingleServer();
    virtual bool Start(const std::string &IP, int Port);

    virtual bool ProcessConnected();
    virtual bool ProcessDisconnected();
    virtual bool ProcessSystemCheckout();
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg);

private:

    bool ProcessLogin(NetworkMessage &Message);
    bool ProcessLogout(NetworkMessage &Message);

    bool SendMessage(int MessageType, NetworkMessage &NetworkMsg, const google::protobuf::Message &ProtobufMsg);

private:

    MySQLDatabase                       m_MySQLDatabase;
    std::shared_ptr<HTTPBaseServer>     m_HTTPServer;
};

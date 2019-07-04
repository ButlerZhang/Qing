#pragma once
#include "Network/SingleEventBaseServer.h"
#include <google/protobuf/message.h>



class SingleServer : public SingleEventBaseServer
{
public:

    SingleServer();
    virtual ~SingleServer();

    virtual bool ProcessConnected();
    virtual bool ProcessDisconnected();
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg);

private:

    bool ProcessLogin(NetworkMessage &Message);
    bool ProcessLogout(NetworkMessage &Message);

    bool SendMessage(int MessageType, NetworkMessage &NetworkMsg, const google::protobuf::Message &ProtobufMsg);
};

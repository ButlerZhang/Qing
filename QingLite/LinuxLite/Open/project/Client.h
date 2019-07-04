#pragma once
#include "Network/SingleEventBaseClient.h"
#include <google/protobuf/message.h>



class Client : public SingleEventBaseClient
{
public:

    Client();
    virtual ~Client();

    virtual bool ProcessConnected();
    virtual bool ProcessDisconnected();
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg);

private:

    bool ProcessLoginResponse(NetworkMessage &NetworkMsg);

    bool SendLogin();
    bool SendLogout();
    bool SendMessage(int MessageType, const google::protobuf::Message &ProtobufMsg);
};

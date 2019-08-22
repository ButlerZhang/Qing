#pragma once
#include "../core/network/SingleEventBaseClient.h"
#include <google/protobuf/message.h>



class Client : public SingleEventBaseClient
{
public:

    Client(long ClientID);
    virtual ~Client();

    virtual bool ProcessCheckout();
    virtual bool ProcessConnected();
    virtual bool ProcessDisconnected();
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg);

private:

    bool ProcessRandom(NetworkMessage &NetworkMsg);
    bool ProcessServerError(NetworkMessage &NetworkMsg);
    bool ProcessLoginResponse(NetworkMessage &NetworkMsg);
    bool ProcessLogoutResponse(NetworkMessage &NetworkMsg);

    bool SendLogin();
    bool SendLogout();
    bool SendRandom();
    bool SendMessage(int MessageType, const google::protobuf::Message &ProtobufMsg);
};

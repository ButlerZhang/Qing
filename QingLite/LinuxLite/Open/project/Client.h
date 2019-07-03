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
    virtual bool ProcessMessage(std::string &MessageString);

private:

    bool ProcessLoginResponse(std::string &MessageString);

    bool SendMessage(int MessageType, const google::protobuf::Message &message);

    bool SendLogin();
    bool SendLogout();
};

#pragma once
#include "Network/SingleEventBaseServer.h"



class SingleServer : public SingleEventBaseServer
{
public:

    SingleServer();
    virtual ~SingleServer();

    virtual bool ProcessConnected();
    virtual bool ProcessDisconnected();
    virtual bool ProcessMessage(MessageHandler::MessageNode &Message);

private:

    bool ProcessLogin(MessageHandler::MessageNode &Message);
    bool ProcessLogout(MessageHandler::MessageNode &Message);
};

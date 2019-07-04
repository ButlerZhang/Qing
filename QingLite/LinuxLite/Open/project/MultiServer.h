#pragma once
#include "Network/MultiEventBaseServer.h"
#include <google/protobuf/message.h>



class MultiServer : public MultiEventBaseServer
{
public:

    MultiServer();
    virtual ~MultiServer();

    virtual bool ProcessConnected(ConnectNode &ConnectedNode);
    virtual bool ProcessDisconnected(ConnectNode &ConnectedNode, short events);
    virtual bool ProcessMessage(ConnectNode &ConnectedNode);

private:

    bool ProcessLogin(ConnectNode &ConnectedNode);
    bool ProcessLogout(ConnectNode &ConnectedNode);

    bool SendMessage(int MessageType, ConnectNode &ConnectedNode, const google::protobuf::Message &ProtobufMsg);
};

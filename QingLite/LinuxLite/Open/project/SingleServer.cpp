#include "SingleServer.h"
#include "Message/project.pb.h"
#include "Message/CodedMessage.h"



SingleServer::SingleServer()
{
}

SingleServer::~SingleServer()
{
}

bool SingleServer::ProcessConnected()
{
    return false;
}

bool SingleServer::ProcessDisconnected()
{
    return false;
}

bool SingleServer::ProcessMessage(const MessageHandler::MessageNode &Message)
{
    google::protobuf::Message* ProtoMessage = DecodeMessage(Message.m_Message);
    if (ProtoMessage != NULL)
    {
        ProtoMessage->PrintDebugString();
        delete ProtoMessage;
        return true;
    }

    return false;
}

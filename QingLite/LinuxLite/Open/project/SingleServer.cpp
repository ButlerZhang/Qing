#include "SingleServer.h"
#include "../../LinuxTools.h"
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

bool SingleServer::ProcessMessage(MessageHandler::MessageNode &Message)
{
    int MessageType = DecodeMessage(Message.m_Message);
    switch (MessageType)
    {
    case Project::MessageType::MT_LOGIN:    return ProcessLogin(Message);
    case Project::MessageType::MT_LOGOUT:   return ProcessLogout(Message);
    default:                                return false;
    }

    //google::protobuf::Message* ProtoMessage = DecodeMessage(Message.m_Message);
    //if (ProtoMessage != NULL)
    //{
    //    ProtoMessage->PrintDebugString();
    //    delete ProtoMessage;
    //    return true;
    //}

    //return false;
}

bool SingleServer::ProcessLogin(MessageHandler::MessageNode &Message)
{
    Project::UserLogin Login;
    if (!Login.ParseFromString(Message.m_Message))
    {
        return false;
    }

    Login.PrintDebugString();

    Project::UserLogin Response;
    Response.set_id(Login.id());
    Response.set_name(Login.name());
    Response.set_password(Login.password());
    Response.set_authority(Login.authority());
    Response.add_nickname(Login.nickname()[0]);
    Response.add_nickname(Login.nickname()[1]);

    Project::MessageHeader *Header = Response.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGIN_RESPONSE);
    Header->set_transmissionid(GetUUID());

    const std::string &DataString = EncodeMessage(Response, Project::MessageType::MT_LOGIN_RESPONSE);
    if (bufferevent_write(Message.m_bufferevent, (void*)DataString.c_str(), DataString.size()) != 0)
    {
        printf("Send failed.\n");
        return false;
    }

    return true;
}

bool SingleServer::ProcessLogout(MessageHandler::MessageNode &Message)
{
    Project::UserLogout LogoutMessage;
    if (!LogoutMessage.ParseFromString(Message.m_Message))
    {
        return false;
    }

    LogoutMessage.PrintDebugString();
    return true;
}

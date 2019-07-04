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

bool SingleServer::ProcessMessage(NetworkMessage &Message)
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

bool SingleServer::ProcessLogin(NetworkMessage &Message)
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

    return SendMessage(Project::MessageType::MT_LOGIN_RESPONSE, Message, Response);
}

bool SingleServer::ProcessLogout(NetworkMessage &Message)
{
    Project::UserLogout LogoutMessage;
    if (!LogoutMessage.ParseFromString(Message.m_Message))
    {
        return false;
    }

    LogoutMessage.PrintDebugString();
    return true;
}

bool SingleServer::SendMessage(int MessageType, NetworkMessage &NetworkMsg, const google::protobuf::Message &ProtobufMsg)
{
    const std::string &DataString = EncodeMessage(ProtobufMsg, MessageType);
    return Send(NetworkMsg, DataString.c_str(), DataString.size());
}

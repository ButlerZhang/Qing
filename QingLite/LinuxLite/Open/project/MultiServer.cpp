#include "MultiServer.h"
#include "Tools/OpenSSLAES.h"
#include "../../LinuxTools.h"
#include "Message/project.pb.h"
#include "Message/CodedMessage.h"



MultiServer::MultiServer()
{
}

MultiServer::~MultiServer()
{
}

bool MultiServer::ProcessConnected(ConnectNode & ConnectedNode)
{
    return false;
}

bool MultiServer::ProcessDisconnected(ConnectNode & ConnectedNode, short events)
{
    return false;
}

bool MultiServer::ProcessMessage(ConnectNode &ConnectedNode)
{
    std::string DecryptDataString = AEScbcDecrypt(ConnectedNode.m_Message, "Butler");
    ConnectedNode.m_Message.swap(DecryptDataString);
    printf("Decrypt: %s\n", ConnectedNode.m_Message.c_str());

    int MessageType = DecodeMessage(ConnectedNode.m_Message);
    switch (MessageType)
    {
    case Project::MessageType::MT_LOGIN:    return ProcessLogin(ConnectedNode);
    case Project::MessageType::MT_LOGOUT:   return ProcessLogout(ConnectedNode);
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

bool MultiServer::ProcessLogin(ConnectNode &ConnectedNode)
{
    Project::UserLogin Login;
    if (!Login.ParseFromString(ConnectedNode.m_Message))
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

    return SendMessage(Project::MessageType::MT_LOGIN_RESPONSE, ConnectedNode, Response);
}

bool MultiServer::ProcessLogout(ConnectNode &ConnectedNode)
{
    Project::UserLogout LogoutMessage;
    if (!LogoutMessage.ParseFromString(ConnectedNode.m_Message))
    {
        return false;
    }

    LogoutMessage.PrintDebugString();
    return true;
}

bool MultiServer::SendMessage(int MessageType, ConnectNode & ConnectedNode, const google::protobuf::Message & ProtobufMsg)
{
    const std::string &DataString = EncodeMessage(ProtobufMsg, MessageType);
    const std::string &SendData = AEScbcEncrypt(DataString, "Butler");
    return Send(ConnectedNode, SendData);
}

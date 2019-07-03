#include "Client.h"
#include "../../LinuxTools.h"
#include "Message/project.pb.h"
#include "Message/CodedMessage.h"



Client::Client()
{
}

Client::~Client()
{
}

bool Client::ProcessConnected()
{
    return SendLogin();
}

bool Client::ProcessDisconnected()
{
    //Add Log
    return false;
}

bool Client::ProcessMessage(std::string &MessageString)
{
    int MessageType = DecodeMessage(MessageString);
    switch (MessageType)
    {
    case Project::MessageType::MT_LOGIN_RESPONSE:   return ProcessLoginResponse(MessageString);
    default:                                        return false;
    }
}

bool Client::ProcessLoginResponse(std::string &MessageString)
{
    Project::UserLogin LoginResponse;
    if (!LoginResponse.ParseFromString(MessageString))
    {
        return false;
    }

    LoginResponse.PrintDebugString();
    return true;
}

bool Client::SendMessage(int MessageType, const google::protobuf::Message & message)
{
    const std::string &DataString = EncodeMessage(message, MessageType);
    return Send((void*)DataString.c_str(), DataString.size());
}

bool Client::SendLogin()
{
    Project::UserLogin Login;
    Login.set_id(1000);
    Login.set_name("Butler");
    Login.set_password("1234567890");
    Login.set_authority("1001000100");
    Login.add_nickname("Sky");
    Login.add_nickname("Sea");

    Project::MessageHeader *Header = Login.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGIN);
    Header->set_transmissionid(GetUUID());

    Login.PrintDebugString();
    return SendMessage(Header->type(), Login);
}

bool Client::SendLogout()
{
    Project::UserLogout Logout;
    Logout.set_id(1000);
    Logout.set_name("Butler");

    Project::MessageHeader *Header = Logout.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGOUT);
    Header->set_transmissionid(GetUUID());

    Logout.PrintDebugString();
    return SendMessage(Header->type(), Logout);
}

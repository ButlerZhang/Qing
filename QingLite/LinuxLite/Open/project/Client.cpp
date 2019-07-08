#include "Client.h"
#include "Tools/AES.h"
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

bool Client::ProcessMessage(NetworkMessage &NetworkMsg)
{
    std::string DecryptDataString = AEScbcDecrypt(NetworkMsg.m_Message, "Butler");
    NetworkMsg.m_Message.swap(DecryptDataString);

    int MessageType = DecodeMessage(NetworkMsg.m_Message);
    switch (MessageType)
    {
    case Project::MessageType::MT_LOGIN_RESPONSE:   return ProcessLoginResponse(NetworkMsg);
    default:                                        return false;
    }
}

bool Client::ProcessLoginResponse(NetworkMessage &NetworkMsg)
{
    printf("Process login response\n");
    Project::UserLogin LoginResponse;
    if (!LoginResponse.ParseFromString(NetworkMsg.m_Message))
    {
        return false;
    }

    LoginResponse.PrintDebugString();
    SendLogout();
    return true;
}

bool Client::SendLogin()
{
    printf("Process login:\n");
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
    printf("Process logout:\n");
    Project::UserLogout Logout;
    Logout.set_id(1000);
    Logout.set_name("Butler");

    Project::MessageHeader *Header = Logout.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGOUT);
    Header->set_transmissionid(GetUUID());

    Logout.PrintDebugString();
    return SendMessage(Header->type(), Logout);
}

bool Client::SendMessage(int MessageType, const google::protobuf::Message &ProtobufMsg)
{
    const std::string &DataString = EncodeMessage(ProtobufMsg, MessageType);
    const std::string &SendData = AEScbcEncrypt(DataString, "Butler");
    //printf("Encrypt : %s\n", SendData.c_str());
    return Send((void*)SendData.c_str(), SendData.size());
}

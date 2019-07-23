#include "Client.h"
#include "../core/tools/BoostLog.h"
#include "../core/tools/OpenSSLAES.h"
#include "../../../LinuxTools.h"
#include "../message/project.pb.h"
#include "../message/CodedMessage.h"



Client::Client()
{
}

Client::~Client()
{
}

bool Client::ProcessConnected()
{
    g_Log.WriteDebug("Process connected.");
    return SendLogin();
}

bool Client::ProcessDisconnected()
{
    g_Log.WriteDebug("Process disconnected.");
    return false;
}

bool Client::ProcessMessage(NetworkMessage &NetworkMsg)
{
    //std::string DecryptDataString = AESDecrypt(NetworkMsg.m_Message, "Butler");
    //NetworkMsg.m_Message.swap(DecryptDataString);

    int MessageType = DecodeMessage(NetworkMsg.m_Message);
    switch (MessageType)
    {
    case Project::MessageType::MT_LOGIN_RESPONSE:   return ProcessLoginResponse(NetworkMsg);
    default:                                        return false;
    }
}

bool Client::ProcessLoginResponse(NetworkMessage &NetworkMsg)
{
    g_Log.WriteDebug("Process login response.");
    Project::UserLogin LoginResponse;
    if (!LoginResponse.ParseFromString(NetworkMsg.m_Message))
    {
        g_Log.WriteError("Login response message parse failed.");
        return false;
    }

    g_Log.WriteDebug(LoginResponse.DebugString());
    return SendLogout();
}

bool Client::SendLogin()
{
    g_Log.WriteDebug("Send Login.");
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

    g_Log.WriteDebug(Login.DebugString());
    return SendMessage(Header->type(), Login);
}

bool Client::SendLogout()
{
    g_Log.WriteDebug("Send Logout.");
    Project::UserLogout Logout;
    Logout.set_id(1000);
    Logout.set_name("Butler");

    Project::MessageHeader *Header = Logout.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGOUT);
    Header->set_transmissionid(GetUUID());

    g_Log.WriteDebug(Logout.DebugString());
    return SendMessage(Header->type(), Logout);
}

bool Client::SendMessage(int MessageType, const google::protobuf::Message &ProtobufMsg)
{
    const std::string &EncodeString = EncodeMessage(ProtobufMsg, MessageType);
    //const std::string &EncryptString = AESEncrypt(EncodeString, "Butler");
    return Send((void*)EncodeString.c_str(), EncodeString.size());
}

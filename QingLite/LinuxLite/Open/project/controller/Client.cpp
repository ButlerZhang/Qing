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

    //test case 1: send fast and more
    //for (int Count = 0; Count < 1000; Count++)
    //{
    //    SendLogin();
    //}
    //return true;
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

    g_Log.WriteDebug("Logout response message:\n" + LoginResponse.DebugString());
    return true;
    //return SendLogout();
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

    //static int Count = 0;
    //Header->set_transmissionid(std::to_string(++Count));

    g_Log.WriteDebug("Login message:\n" + Login.DebugString());
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

    g_Log.WriteDebug("Logout message:\n" + Logout.DebugString());
    return SendMessage(Header->type(), Logout);
}

bool Client::SendMessage(int MessageType, const google::protobuf::Message &ProtobufMsg)
{
    const std::string &EncodeString = EncodeMessage(ProtobufMsg, MessageType);
    //const std::string &EncryptString = AESEncrypt(EncodeString, "Butler");
    return Send((void*)EncodeString.c_str(), EncodeString.size());

    //test case 1: slow send
    //for (std::string::size_type Index = 0; Index < EncodeString.size(); Index++)
    //{
    //    g_Log.WriteDebug(BoostFormat("Send message test count = %d.", Index));
    //    Send(&EncodeString[Index], 1);
    //    sleep(1);
    //}
    //return true;
}

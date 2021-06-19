#include "Client.h"
#include "../message/CodedMessage.h"
#include "../message/project.pb.h"
#include "../../../LinuxTools.h"



Client::Client(long ClientID) : SingleEventBaseClient(ClientID)
{
}

Client::~Client()
{
}

bool Client::ProcessCheckout()
{
    if (SingleEventBaseClient::ProcessCheckout())
    {
        bool IsOkToSendDataRandomly = true; //TODO, config
        if (IsOkToSendDataRandomly)
        {
            SendRandom();
        }
    }

    return true;
}

bool Client::ProcessConnected()
{
    g_Log.WriteDebug("Client process connected.");
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
    g_Log.WriteDebug("Client process disconnected.");
    return false;
}

bool Client::ProcessMessage(NetworkMessage &NetworkMsg)
{
    int MessageType = DecodeMessage(NetworkMsg.m_Message);
    switch (MessageType)
    {
    case Project::MessageType::MT_ERROR:            return ProcessServerError(NetworkMsg);
    case Project::MessageType::MT_RANDOM:           return ProcessRandom(NetworkMsg);
    case Project::MessageType::MT_LOGIN_RESPONSE:   return ProcessLoginResponse(NetworkMsg);
    case Project::MessageType::MT_LOGOUT_RESPONSE:  return ProcessLogoutResponse(NetworkMsg);
    default:                                        return false;
    }
}

bool Client::ProcessRandom(NetworkMessage &NetworkMsg)
{
    g_Log.WriteDebug("Client process random response message.");

    Project::RandomMessage RandomMsg;
    if (!RandomMsg.ParseFromString(NetworkMsg.m_Message))
    {
        g_Log.WriteError("Random message parse failed.");
        return false;
    }

    g_Log.WriteDebug("Random response message:\n" + RandomMsg.DebugString());
    return true;
}

bool Client::ProcessServerError(NetworkMessage & NetworkMsg)
{
    g_Log.WriteDebug("Client process server error.");

    Project::ServerError Error;
    if (!Error.ParseFromString(NetworkMsg.m_Message))
    {
        g_Log.WriteError("Serer error message parse failed.");
        return false;
    }

    g_Log.WriteDebug("Server error message:\n" + Error.DebugString());
    return true;
}

bool Client::ProcessLoginResponse(NetworkMessage &NetworkMsg)
{
    g_Log.WriteDebug("Client process login response.");

    Project::UserLogin LoginResponse;
    if (!LoginResponse.ParseFromString(NetworkMsg.m_Message))
    {
        g_Log.WriteError("Login response message parse failed.");
        return false;
    }

    g_Log.WriteDebug("Login response message:\n" + LoginResponse.DebugString());
    return true;
}

bool Client::ProcessLogoutResponse(NetworkMessage &NetworkMsg)
{
    g_Log.WriteDebug("Client process logout response.");

    Project::UserLogout LogoutResponse;
    if (!LogoutResponse.ParseFromString(NetworkMsg.m_Message))
    {
        g_Log.WriteError("Logout response message parse failed.");
        return false;
    }

    g_Log.WriteDebug("Logout response message:\n" + LogoutResponse.DebugString());
    return true;
}

bool Client::SendLogin()
{
    std::string ClientName("Client-" + std::to_string(GetClientID()));

    Project::UserLogin Login;
    Login.set_id(1000);
    Login.set_name(ClientName);
    Login.set_password("1234567890");
    Login.set_authority("1001000100");
    Login.add_nickname("Sky");
    Login.add_nickname("Sea");

    Project::MessageHeader *Header = Login.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGIN);
    //Header->set_transmissionid(GetUUID());

    g_Log.WriteDebug("Client send login message:\n" + Login.DebugString());
    return SendMessage(Header->type(), Login);
}

bool Client::SendLogout()
{
    std::string ClientName("Client-" + std::to_string(GetClientID()));

    Project::UserLogout Logout;
    Logout.set_id(1000);
    Logout.set_name(ClientName);

    Project::MessageHeader *Header = Logout.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGOUT);
    Header->set_transmissionid(GetUUID());

    g_Log.WriteDebug("Client send logout message:\n" + Logout.DebugString());
    return SendMessage(Header->type(), Logout);
}

bool Client::SendRandom()
{
    static int64_t Sequence = 0;

    Project::RandomMessage RandomMsg;
    RandomMsg.set_serversequence(-1);
    RandomMsg.set_clientsequence(++Sequence);
    RandomMsg.set_randomdescriptor("Client-" + std::to_string(GetClientID()) + " send sequence = " + std::to_string(Sequence));

    Project::MessageHeader *Header = RandomMsg.mutable_header();
    Header->set_type(Project::MessageType::MT_RANDOM);
    Header->set_transmissionid(GetUUID());

    g_Log.WriteDebug("Client send random message:\n" + RandomMsg.DebugString());
    return SendMessage(Project::MessageType::MT_RANDOM, RandomMsg);
}

bool Client::SendMessage(int MessageType, const google::protobuf::Message &ProtobufMsg)
{
    const std::string &EncodeString = EncodeMessage(ProtobufMsg, MessageType);
    return Send((void*)EncodeString.c_str(), EncodeString.size());

    //test case 2: slow send
    //for (std::string::size_type Index = 0; Index < EncodeString.size(); Index++)
    //{
    //    g_Log.WriteDebug(BoostFormat("Send message test count = %d.", Index));
    //    Send(&EncodeString[Index], 1);
    //    sleep(1);
    //}
    //return true;
}

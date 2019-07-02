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
    Project::User LoginMessage;
    LoginMessage.set_id(1000);
    LoginMessage.set_name("Butler");
    LoginMessage.set_password("1234567890");
    LoginMessage.set_authority("1001000100");
    LoginMessage.add_nickname("Sky");
    LoginMessage.add_nickname("Sea");

    Project::MessageHeader *Header = LoginMessage.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGOUT);
    Header->set_transmissionid(GetUUID());

    printf("Login user information:\n");
    LoginMessage.PrintDebugString();
    printf("\n");

    const std::string &DataString = EncodeMessage(LoginMessage);
    return Send((void*)DataString.c_str(), DataString.size());
}

bool Client::ProcessDisconnected()
{
    //Add Log
    return false;
}

bool Client::ProcessMessage(void * Data, size_t Size)
{
    return false;
}

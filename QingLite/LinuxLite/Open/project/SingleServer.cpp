#include "SingleServer.h"
#include "Message/project.pb.h"



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
    Project::User CheckUser;
    if(!CheckUser.ParseFromString(Message.m_Message))
    {
        printf("ERROR: parse from istream failed.\n");
        return false;
    }

    printf("\nCheck user information:\n");
    CheckUser.PrintDebugString();
    printf("\nType=%d\n", CheckUser.mutable_header()->type());
    return true;
}

#include "SingleServer.h"
#include "../../../LinuxTools.h"
#include "../../../../Common/Boost/BoostLog.h"
#include "../core/network/ThreadNoticeQueue.h"
#include "../message/project.pb.h"
#include "../message/CodedMessage.h"
#include "../Config.h"



SingleServer::SingleServer()
{
    m_HandlerVector.push_back(std::make_shared<TCPHandler>());
}

SingleServer::~SingleServer()
{
    m_HandlerVector.clear();
}

bool SingleServer::Start(const std::string &IP, int Port)
{
    if (m_SMIBDB.Connect(g_Config.m_DBHost.c_str(),
        g_Config.m_DBUser.c_str(),
        g_Config.m_DBPassword.c_str(),
        g_Config.m_DBName.c_str(),
        g_Config.m_DBPort) == false)
    {
        g_Log.WriteError("Single Server connnect SMIB database failed.");
        return false;
    }

    g_Log.WriteDebug("Single Server connect SMIB database succeed.");
    return SingleEventBaseServer::Start(IP, Port);
}

bool SingleServer::ProcessCheckout()
{
    if (!m_SMIBDB.Isconnected())
    {
        g_Log.WriteError("Single Server SMIB database is disconnected.");

        if (m_SMIBDB.Reconnect())
        {
            g_Log.WriteInfo("Single Server SMIB database reconnect succeed.");
        }
        else
        {
            g_Log.WriteDebug("Single Server SMIB database reconnect failed.");
        }
    }

    g_Log.Flush();
    return true;
}

bool SingleServer::ProcessConnected()
{
    g_Log.WriteDebug("Single Server process connected.");
    return true;
}

bool SingleServer::ProcessDisconnected()
{
    g_Log.WriteDebug("Single Server process disconnected.");
    return true;
}

bool SingleServer::ProcessThreadNoticeQueue()
{
    std::string JsonString;
    if (!g_ThreadNoticeQueue.PopMessage(JsonString))
    {
        g_Log.WriteDebug("Single Server process notice queue, pop message failed.");
        return false;
    }
    if (!HasClient())
    {
        g_Log.WriteDebug("Single Server process notice queue, no smib client.");
        return false;
    }
    g_Log.WriteDebug(BoostFormat("Single Server process notice queue: %s", JsonString.c_str()));
        Project::ServerError ServerPublic;
        ServerPublic.set_errortype(12341234);
        ServerPublic.set_errordescriptor("Http server: " + JsonString);
        Project::MessageHeader *Header = ServerPublic.mutable_header();
        Header->set_type(Project::MessageType::MT_ERROR);
        Header->set_transmissionid(GetUUID());
        return SendMessage(Project::MessageType::MT_ERROR, ServerPublic);
    }

bool SingleServer::ProcessMessage(NetworkMessage &NetworkMsg)
{
    NetworkMsg.m_MessageType = DecodeMessage(NetworkMsg.m_Message);
    for (std::vector<std::shared_ptr<TCPHandler>>::size_type Index = 0; Index < m_HandlerVector.size(); Index++)
    {
        if (m_HandlerVector[Index]->ProcessMessage(NetworkMsg))
        {
            return true;
        }
    }

    return false;

    //google::protobuf::Message* ProtoMessage = DecodeMessage(Message.m_Message);
    //if (ProtoMessage != NULL)
    //{
    //    ProtoMessage->PrintDebugString();
    //    delete ProtoMessage;
    //    return true;
    //}

    //return false;
}

bool SingleServer::SendMessage(int MessageType, const google::protobuf::Message & ProtobufMsg)
{
    const std::string &DataString = EncodeMessage(ProtobufMsg, MessageType);
    return Send(DataString.c_str(), DataString.size());
}

bool SingleServer::SendMessage(int MessageType, NetworkMessage &NetworkMsg, const google::protobuf::Message &ProtobufMsg)
{
    const std::string &DataString = EncodeMessage(ProtobufMsg, MessageType);
    return Send(NetworkMsg, DataString.c_str(), DataString.size());
}

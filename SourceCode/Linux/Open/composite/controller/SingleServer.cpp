#include "SingleServer.h"
#include "../../../../Common/Boost/BoostLog.h"
#include "../../../LinuxTools.h"
#include "../core/network/ThreadNoticeQueue.h"
#include "../handler/tcphandler/TCPHandler.h"
#include "../message/project.pb.h"
#include "../message/CodedMessage.h"
#include "HTTPServer.h"
#include "../Config.h"



SingleServer::SingleServer()
{
    m_HandlerVector.push_back(std::make_shared<TCPHandler>());
}

SingleServer::~SingleServer()
{
    m_HandlerVector.clear();
}

bool SingleServer::ProcessCheckout()
{
    //step 1: load ethernet information
    if (!m_Ethernet.Initialize())
    {
        return false;
    }

    //step 2: load configuration information
    if (!g_Config.IsLoadSucceed())
    {
        if (!g_Config.LoadConfig())
        {
            return false;
        }

        if (!CreateListener(g_Config.m_ServerIP, g_Config.m_SMIBPort))
        {
            return false;
        }

        g_HTTPServer.Start(g_Config.m_ServerIP, g_Config.m_HTTPPort);
        return true;
    }

    //step 3: check database connection
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

    //step 4: flush log file.
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

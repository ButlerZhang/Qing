#include "SingleServer.h"
#include "../../../LinuxTools.h"
#include "../core/tools/BoostLog.h"
#include "../message/project.pb.h"
#include "../message/CodedMessage.h"
#include "../Config.h"



SingleServer::SingleServer()
{
}

SingleServer::~SingleServer()
{
}

SingleServer & SingleServer::GetInstance()
{
    static SingleServer g_SingleServerInstance;
    return g_SingleServerInstance;
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

bool SingleServer::ProcessMessage(NetworkMessage &NetworkMsg)
{
    int MessageType = DecodeMessage(NetworkMsg.m_Message);
    switch (MessageType)
    {
    case Project::MessageType::MT_LOGIN:    return ProcessLogin(NetworkMsg);
    case Project::MessageType::MT_LOGOUT:   return ProcessLogout(NetworkMsg);
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

bool SingleServer::ProcessLogin(NetworkMessage &NetworkMsg)
{
    Project::UserLogin Response;
    g_Log.WriteDebug("Single Server process login.");

    Project::UserLogin Login;
    if (!Login.ParseFromString(NetworkMsg.m_Message))
    {
        const std::string &LogString = BoostFormat("Single Server login message parse failed, socket = %d.", NetworkMsg.m_Socket);
        g_Log.WriteError(LogString);

        Response.set_name(LogString);
        Response.set_id(NetworkMsg.m_Socket);
        return SendMessage(Project::MessageType::MT_LOGIN_RESPONSE, NetworkMsg, Response);
    }

    g_Log.WriteDebug("Single Server login message\n" + Login.DebugString());

    const std::string &InsertSQL = BoostFormat("INSERT IGNORE INTO events_log (code, description, date_time) VALUES(%d,'%s', %s)",
        Login.id(), "user login", "NOW()");
    if (!m_SMIBDB.ExecuteQuery(InsertSQL.c_str()))
    {
        g_Log.WriteError(BoostFormat("Single Server insert database falied: %s", InsertSQL.c_str()));

        Response.set_name(InsertSQL);
        Response.set_id(NetworkMsg.m_Socket);
        return SendMessage(Project::MessageType::MT_LOGIN_RESPONSE, NetworkMsg, Response);
    }

    Response.set_id(Login.id());
    Response.set_name(Login.name());
    Response.set_password(Login.password());
    Response.set_authority(Login.authority());
    Response.add_nickname(Login.nickname()[0]);
    Response.add_nickname(Login.nickname()[1]);

    Project::MessageHeader *Header = Response.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGIN_RESPONSE);
    Header->set_transmissionid(Login.mutable_header()->transmissionid());

    g_Log.WriteDebug("Single Server login response message\n" + Response.DebugString());
    return SendMessage(Project::MessageType::MT_LOGIN_RESPONSE, NetworkMsg, Response);
}

bool SingleServer::ProcessLogout(NetworkMessage &NetworkMsg)
{
    g_Log.WriteDebug("Single Server process logout.");

    Project::UserLogout Logout;
    if (!Logout.ParseFromString(NetworkMsg.m_Message))
    {
        g_Log.WriteError("Single Server logout message parse failed.");
        return false;
    }

    g_Log.WriteDebug("Single Server logout message\n" + Logout.DebugString());

    Project::UserLogout Response;
    Response.set_id(Logout.id());
    Response.set_name(Logout.name());

    Project::MessageHeader *Header = Response.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGOUT_RESPONSE);
    Header->set_transmissionid(Logout.mutable_header()->transmissionid());

    g_Log.WriteDebug("Single Server logout response message\n" + Response.DebugString());
    return SendMessage(Project::MessageType::MT_LOGOUT_RESPONSE, NetworkMsg, Response);
}

bool SingleServer::SendMessage(int MessageType, NetworkMessage &NetworkMsg, const google::protobuf::Message &ProtobufMsg)
{
    const std::string &DataString = EncodeMessage(ProtobufMsg, MessageType);
    return Send(NetworkMsg, DataString.c_str(), DataString.size());
}

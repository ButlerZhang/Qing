#include "SingleServer.h"
#include "../../../LinuxTools.h"
#include "../core/tools/BoostLog.h"
#include "../core/tools/OpenSSLAES.h"
#include "../message/project.pb.h"
#include "../message/CodedMessage.h"
#include "../Config.h"



SingleServer g_SingleServer;

SingleServer::SingleServer()
{
}

SingleServer::~SingleServer()
{
}

bool SingleServer::Start(const std::string &IP, int Port)
{
    if (m_SMIBDB.Connect(g_Config.m_DBHost.c_str(),
        g_Config.m_DBUser.c_str(),
        g_Config.m_DBPassword.c_str(),
        g_Config.m_DBName.c_str(),
        g_Config.m_DBPort) == false)
    {
        BoostLog::WriteError("Connnect SMIB database failed.");
        return false;
    }

    BoostLog::WriteDebug("Connect SMIB database succeed.");
    return SingleEventBaseServer::Start(IP, Port);
}

bool SingleServer::ProcessCheckout()
{
    //BoostLog::WriteDebug("Process single server chekout.");

    if (!m_SMIBDB.Isconnected())
    {
        BoostLog::WriteError("SMIB database is disconnected.");

        if (m_SMIBDB.Reconnect())
        {
            BoostLog::WriteInfo("SMIB database reconnect succeed.");
        }
        else
        {
            BoostLog::WriteDebug("SMIB database reconnect failed.");
        }
    }

    return true;
}

bool SingleServer::ProcessConnected()
{
    BoostLog::WriteDebug("Process connected.");
    return false;
}

bool SingleServer::ProcessDisconnected()
{
    BoostLog::WriteDebug("Process disconnected.");
    return false;
}

bool SingleServer::ProcessMessage(NetworkMessage &NetworkMsg)
{
    //std::string DecryptString = AESDecrypt(NetworkMsg.m_Message, "Butler");
    //NetworkMsg.m_Message.swap(DecryptString);
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

bool SingleServer::ProcessLogin(NetworkMessage &Message)
{
    BoostLog::WriteDebug("Process login.");
    Project::UserLogin Login;
    if (!Login.ParseFromString(Message.m_Message))
    {
        BoostLog::WriteError("Login message parse failed.");
        return false;
    }

    BoostLog::WriteDebug(Login.DebugString());

    if (!m_SMIBDB.Isconnected())
    {
        BoostLog::WriteError("Database is disconnected.");
        return false;
    }

    const std::string &InsertSQL = BoostFormat("INSERT IGNORE INTO events_log (code, description, date_time) VALUES(%d,'%s', %s)",
        GetRandomUIntInRange(0, INT_MAX), "user login", "NOW()");
    if (!m_SMIBDB.ExecuteQuery(InsertSQL.c_str()))
    {
        BoostLog::WriteError(BoostFormat("Insert falied: %s", InsertSQL.c_str()));
        return false;
    }

    Project::UserLogin Response;
    Response.set_id(Login.id());
    Response.set_name(Login.name());
    Response.set_password(Login.password());
    Response.set_authority(Login.authority());
    Response.add_nickname(Login.nickname()[0]);
    Response.add_nickname(Login.nickname()[1]);

    Project::MessageHeader *Header = Response.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGIN_RESPONSE);
    Header->set_transmissionid(GetUUID());

    BoostLog::WriteDebug(Response.DebugString());
    return SendMessage(Project::MessageType::MT_LOGIN_RESPONSE, Message, Response);
}

bool SingleServer::ProcessLogout(NetworkMessage &Message)
{
    BoostLog::WriteDebug("Process logout.");
    Project::UserLogout Logout;
    if (!Logout.ParseFromString(Message.m_Message))
    {
        BoostLog::WriteError("Logout message parse failed.");
        return false;
    }

    BoostLog::WriteDebug(Logout.DebugString());
    return true;
}

bool SingleServer::SendMessage(int MessageType, NetworkMessage &NetworkMsg, const google::protobuf::Message &ProtobufMsg)
{
    const std::string &DataString = EncodeMessage(ProtobufMsg, MessageType);
    //const std::string &SendData = AESEncrypt(DataString, "Butler");
    return Send(NetworkMsg, DataString.c_str(), DataString.size());
}

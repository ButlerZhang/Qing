#include "SingleServer.h"
#include "Tools/BoostLog.h"
#include "Tools/OpenSSLAES.h"
#include "../../LinuxTools.h"
#include "Message/project.pb.h"
#include "Message/CodedMessage.h"



SingleServer::SingleServer()
{
}

SingleServer::~SingleServer()
{
}

bool SingleServer::Start(const std::string &IP, int Port)
{
    m_HTTPServer = std::make_shared<HTTPBaseServer>();
    if (!m_HTTPServer->BindBaseEvent(GetEventBase()))
    {
        BoostLog::WriteError("HTTP server bind event base failed.");
        return false;
    }

    std::string WorkDirectory;
    WorkDirectory.append("jpc-web");
    if (!m_HTTPServer->Start(IP, Port + 1, WorkDirectory))
    {
        BoostLog::WriteError("http server start failed.");
        return false;
    }

    if (!m_MySQLDatabase.Connect("192.168.3.126", "root", "root", "jpc", 3306))
    {
        BoostLog::WriteError("Connnect database failed.");
        return false;
    }

    BoostLog::WriteDebug("Connect database succeed.");
    return SingleEventBaseServer::Start(IP, Port);
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

bool SingleServer::ProcessSystemCheckout()
{
    //BoostLog::WriteDebug("Process system chekout.");

    if (!m_MySQLDatabase.Isconnected())
    {
        BoostLog::WriteError("Database is disconnected.");
        if (m_MySQLDatabase.Reconnect())
        {
            BoostLog::WriteDebug("Database reconnect failed.");
        }
    }


    return true;
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

    if (!m_MySQLDatabase.Isconnected())
    {
        BoostLog::WriteError("Database is disconnected.");
        return false;
    }

    const std::string &InsertSQL = BoostFormat("INSERT IGNORE INTO events_log (code, description, date_time) VALUES(%d,'%s', %s)",
        GetRandomUIntInRange(0, INT_MAX), "user login", "NOW()");
    if (!m_MySQLDatabase.ExecuteQuery(InsertSQL.c_str()))
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

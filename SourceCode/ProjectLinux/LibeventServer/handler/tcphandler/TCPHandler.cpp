#include "TCPHandler.h"
#include "../../message/project.pb.h"
#include "../../controller/SingleServer.h"
#include "../../../../Share/Boost/BoostLog.h"



TCPHandler::TCPHandler()
{
}

TCPHandler::~TCPHandler()
{
}

bool TCPHandler::ProcessMessage(NetworkMessage &NetworkMsg)
{
    switch (NetworkMsg.m_MessageType)
    {
    case Project::MessageType::MT_RANDOM:           return ProcessRandom(NetworkMsg);
    case Project::MessageType::MT_LOGIN:            return ProcessLogin(NetworkMsg);
    case Project::MessageType::MT_LOGOUT:           return ProcessLogout(NetworkMsg);
    default:                                        return false;
    }
}

bool TCPHandler::ProcessRandom(NetworkMessage &NetworkMsg)
{
    g_Log.WriteDebug("Single Server process random message.");

    Project::RandomMessage RandomMsg;
    if (!RandomMsg.ParseFromString(NetworkMsg.m_Message))
    {
        g_Log.WriteError("Single Server random message parse failed.");
        return false;
    }

    g_Log.WriteDebug("Single Server random message:\n" + RandomMsg.DebugString());

    Project::RandomMessage Response = RandomMsg;
    Response.set_serversequence(RandomMsg.clientsequence());

    g_Log.WriteDebug("Single Server random response message:\n" + Response.DebugString());
    return g_SingleServer.SendMessage(Project::MessageType::MT_RANDOM, NetworkMsg, Response);
}

bool TCPHandler::ProcessLogin(NetworkMessage &NetworkMsg)
{
    g_Log.WriteDebug("Single Server process login message.");

    Project::UserLogin Login;
    if (!Login.ParseFromString(NetworkMsg.m_Message))
    {
        g_Log.WriteError("Single Server login message parse failed.");
        return false;
    }

    g_Log.WriteDebug("Single Server login message:\n" + Login.DebugString());

    const std::string &InsertSQL = BoostFormat("INSERT IGNORE INTO events_log (code, description, date_time) VALUES(%d,'%s', %s)",
        Login.id(), "user login", "NOW()");
    if (!g_SingleServer.GetDB().ExecuteQuery(InsertSQL.c_str()))
    {
        g_Log.WriteError(BoostFormat("Single Server insert database falied: %s", InsertSQL.c_str()));
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
    Header->set_transmissionid(Login.mutable_header()->transmissionid());

    g_Log.WriteDebug("Single Server login response message:\n" + Response.DebugString());
    return g_SingleServer.SendMessage(Project::MessageType::MT_LOGIN_RESPONSE, NetworkMsg, Response);
}

bool TCPHandler::ProcessLogout(NetworkMessage &NetworkMsg)
{
    g_Log.WriteDebug("Single Server process logout message.");

    Project::UserLogout Logout;
    if (!Logout.ParseFromString(NetworkMsg.m_Message))
    {
        g_Log.WriteError("Single Server logout message parse failed.");
        return false;
    }

    g_Log.WriteDebug("Single Server logout message:\n" + Logout.DebugString());

    Project::UserLogout Response;
    Response.set_id(Logout.id());
    Response.set_name(Logout.name());

    Project::MessageHeader *Header = Response.mutable_header();
    Header->set_type(Project::MessageType::MT_LOGOUT_RESPONSE);
    Header->set_transmissionid(Logout.mutable_header()->transmissionid());

    g_Log.WriteDebug("Single Server logout response message:\n" + Response.DebugString());
    return g_SingleServer.SendMessage(Project::MessageType::MT_LOGOUT_RESPONSE, NetworkMsg, Response);
}
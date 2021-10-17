#include "HTTPSession.h"
#include "../../../LinuxTools.h"
#include "../../../Share/Boost/BoostLog.h"
#include<boost/date_time/posix_time/posix_time.hpp>



HTTPSession::HTTPSession()
{
    m_InvalidSessionID = GetUUID();
    m_SessionMap[m_InvalidSessionID].m_UserID = 0;
    m_SessionMap[m_InvalidSessionID].m_Authority = 0;
}

HTTPSession::~HTTPSession()
{
    m_SessionMap.clear();
}

bool HTTPSession::IsSessionExisted(const std::string &SessionID) const
{
    std::map<std::string, SessionNode>::const_iterator it = m_SessionMap.find(SessionID);
    if (it != m_SessionMap.end())
    {
        g_Log.WriteError(BoostFormat("HTTP session id = %s was existed.", SessionID.c_str()));
        return true;
    }

    return false;
}

bool HTTPSession::IsSessionTimeout(const std::string &SessionID) const
{
    std::map<std::string, SessionNode>::const_iterator it = m_SessionMap.find(SessionID);
    if (it == m_SessionMap.end())
    {
        g_Log.WriteError(BoostFormat("HTTP session check time out, can not find id = %s.", SessionID.c_str()));
        return true;
    }

    boost::posix_time::ptime Start(boost::posix_time::from_iso_string(it->second.m_LastUpdateTime));
    boost::posix_time::ptime Stop = boost::posix_time::second_clock::local_time();

    boost::posix_time::millisec_posix_time_system_config::time_duration_type Elapse = Stop - Start;
    int64_t ElapseSeconds = Elapse.total_seconds();
    g_Log.WriteDebug(BoostFormat("HTTP session check time out, elapse total seconds = %lld", ElapseSeconds));

    bool IsTimeOut = (ElapseSeconds - (30 * 60)) > 0;
    return IsTimeOut;
}

bool HTTPSession::DeleteSession(const std::string &SessionID)
{
    std::map<std::string, SessionNode>::iterator it = m_SessionMap.find(SessionID);
    if (it == m_SessionMap.end())
    {
        g_Log.WriteError(BoostFormat("HTTP session delete, can not find id = %s.", SessionID.c_str()));
        return false;
    }

    m_SessionMap.erase(it);
    g_Log.WriteError(BoostFormat("HTTP session id = %s deleted.", SessionID.c_str()));
    return true;
}

bool HTTPSession::UpdateSessionTime(const std::string &SessionID)
{
    std::map<std::string, SessionNode>::iterator it = m_SessionMap.find(SessionID);
    if (it == m_SessionMap.end())
    {
        g_Log.WriteError(BoostFormat("HTTP session udate time can not find id = %s.", SessionID.c_str()));
        return false;
    }

    it->second.m_LastUpdateTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
    g_Log.WriteError(BoostFormat("HTTP session id = %s udate time = %s.", SessionID.c_str(), it->second.m_LastUpdateTime.c_str()));
    return true;
}

int HTTPSession::GetAuthority(const std::string &SessionID) const
{
    std::map<std::string, SessionNode>::const_iterator it = m_SessionMap.find(SessionID);
    if (it == m_SessionMap.end())
    {
        g_Log.WriteError(BoostFormat("HTTP session get authority, can not find id = %s.", SessionID.c_str()));
        return 0;
    }

    return it->second.m_Authority;
}

int HTTPSession::GetUserID(const std::string &SessionID) const
{
    std::map<std::string, SessionNode>::const_iterator it = m_SessionMap.find(SessionID);
    if (it == m_SessionMap.end())
    {
        g_Log.WriteError(BoostFormat("HTTP session get user id, can not find session id = %s.", SessionID.c_str()));
        return 0;
    }

    return it->second.m_UserID;
}

const std::string & HTTPSession::GetUserName(const std::string & SessionID) const
{
    std::map<std::string, SessionNode>::const_iterator it = m_SessionMap.find(SessionID);
    if (it == m_SessionMap.end())
    {
        g_Log.WriteError(BoostFormat("HTTP session get user name, can not find id = %s.", SessionID.c_str()));
        return m_SessionMap.find(m_InvalidSessionID)->second.m_UserName;
    }

    return it->second.m_UserName;
}

const std::string& HTTPSession::GenerateSession(const std::string &UserName, int Authority, int UserID)
{
    SessionNode NewNode;
    NewNode.m_Authority = Authority;
    NewNode.m_UserName = UserName;
    NewNode.m_SessionID = GetUUID();
    NewNode.m_LastUpdateTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
    NewNode.m_UserID = UserID;

    m_SessionMap[NewNode.m_SessionID] = NewNode;
    g_Log.WriteDebug(BoostFormat("HTTP session add new session id = %s, user = %s, update time = %s",
        NewNode.m_SessionID.c_str(), UserName.c_str(), NewNode.m_LastUpdateTime.c_str()));

    return m_SessionMap[NewNode.m_SessionID].m_SessionID;
}

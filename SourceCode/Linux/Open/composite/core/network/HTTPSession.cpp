#include "HTTPSession.h"
#include "../../../../LinuxTools.h"
#include "../../../../../Common/Boost/BoostLog.h"
#include<boost/date_time/posix_time/posix_time.hpp>



HTTPSession::HTTPSession()
{
}

HTTPSession::~HTTPSession()
{
}

bool HTTPSession::IsSessionExisted(const std::string &SessionID) const
{
    std::map<std::string, SessionNode>::const_iterator it = m_SessionMap.find(SessionID);
    if (it != m_SessionMap.end())
    {
        g_Log.WriteError(BoostFormat("HTTP session id = %s existed.", SessionID.c_str()));
        return true;
    }

    return false;
}

bool HTTPSession::IsSessionTimeout(const std::string &SessionID) const
{
    std::map<std::string, SessionNode>::const_iterator it = m_SessionMap.find(SessionID);
    if (it == m_SessionMap.end())
    {
        g_Log.WriteError(BoostFormat("HTTP session check time out can not find id = %s.", SessionID.c_str()));
        return true;
    }

    boost::posix_time::ptime Start(boost::posix_time::from_iso_string(it->second.m_LastUpdateTime));
    boost::posix_time::ptime Stop = boost::posix_time::second_clock::local_time();

    boost::posix_time::millisec_posix_time_system_config::time_duration_type Elapse = Stop - Start;
    bool IsTimeOut = (Elapse.total_seconds() - (30 * 60)) > 0;

    return IsTimeOut;
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

std::string HTTPSession::GenerateSession(const std::string &UserName, const std::string &Password)
{
    SessionNode NewNode;
    NewNode.m_Authority = 0;
    NewNode.m_UserName = UserName;
    NewNode.m_Password = Password;
    NewNode.m_SessionID = GetUUID();
    NewNode.m_LastUpdateTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());

    m_SessionMap[NewNode.m_SessionID] = NewNode;
    g_Log.WriteDebug(BoostFormat("HTTP session add new session id = %s, user = %s, update time = %s",
        NewNode.m_SessionID.c_str(), UserName.c_str(), NewNode.m_LastUpdateTime.c_str()));

    return NewNode.m_SessionID;
}

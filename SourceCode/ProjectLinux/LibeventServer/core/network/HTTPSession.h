#pragma once
#include <map>
#include <string>



class HTTPSession
{
public:

    HTTPSession();
    ~HTTPSession();

    bool IsSessionExisted(const std::string &SessionID) const;
    bool IsSessionTimeout(const std::string &SessionID) const;

    bool DeleteSession(const std::string &SessionID);
    bool UpdateSessionTime(const std::string &SessionID);

    int  GetAuthority(const std::string &SessionID) const;
    int  GetUserID(const std::string &SessionID) const;

    const std::string& GetUserName(const std::string &SessionID) const;
    const std::string& GenerateSession(const std::string &UserName, int Authority,int UserID);

private:

    struct SessionNode
    {
        int                                      m_UserID;
        int                                      m_Authority;
        std::string                              m_SessionID;
        std::string                              m_UserName;
        std::string                              m_LastUpdateTime;
    };

private:

    std::string                                  m_InvalidSessionID;
    std::map<std::string, SessionNode>           m_SessionMap;
};

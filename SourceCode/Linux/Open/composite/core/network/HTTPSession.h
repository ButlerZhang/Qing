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

    bool UpdateSessionTime(const std::string &SessionID);
    std::string GenerateSession(const std::string &UserName, const std::string &Password, int Authority);

    int  GetAuthority(const std::string &SessionID) const;

private:

    struct SessionNode
    {
        int                                      m_Authority;
        std::string                              m_SessionID;
        std::string                              m_UserName;
        std::string                              m_Password;
        std::string                              m_LastUpdateTime;
    };

private:

    std::map<std::string, SessionNode>           m_SessionMap;
};

#include "UserHandler.h"
#include "DatabaseManager.h"
#include "../core/tools/BoostLog.h"
#include <event.h>
#include <event2/http.h>



UserHandler::UserHandler()
{
}

UserHandler::~UserHandler()
{
}

bool UserHandler::ProcessLogin(evhttp_request * Request)
{
    if (!ParsePostData(GetPostData(Request)))
    {
        return false;
    }

    const std::string &UserName = m_JsonTree.get<std::string>("Name");
    const std::string &Password = m_JsonTree.get<std::string>("Password");
    BoostLog::WriteDebug(BoostFormat("User name = %s, Password = %s", UserName.c_str(), Password.c_str()));

    MySQLDataSet DataSet;
    std::string SQLString(BoostFormat("SELECT * FROM user WHERE name = '%s'", UserName.c_str()));
    if (!g_DBManager.GetHTTPDB()->ExecuteQuery(SQLString.c_str(), &DataSet))
    {
        BoostLog::WriteDebug(BoostFormat("Execute query failed: %s", SQLString.c_str()));
        return false;
    }

    if (DataSet.GetRecordCount() <= 0)
    {
        BoostLog::WriteDebug(BoostFormat("Can not find user = %s", UserName.c_str()));
        return false;
    }

    int IsLock = 0, AuthorityID = 0;
    std::string DBPassword, CreateTime, LastUpdateTime;
    if(!DataSet.GetValue("password", DBPassword) ||
       !DataSet.GetValue("create_time", CreateTime) ||
       !DataSet.GetValue("last_update_time", LastUpdateTime) ||
       !DataSet.GetValue("is_lock", IsLock) ||
        !DataSet.GetValue("authority_id", AuthorityID))
    {
        BoostLog::WriteDebug("Get user value falied.");
        return false;
    }

    if (Password != DBPassword)
    {
        BoostLog::WriteDebug("Password does not match");
        return false;
    }

    if (CreateTime == LastUpdateTime)
    {
        BoostLog::WriteDebug("Login failed, please change your password for the first login!");
        return false;
    }

    if (IsLock)
    {
        BoostLog::WriteDebug("Login failed, user is locked, please contact the administrators!");
        return false;
    }

    BoostLog::WriteDebug(BoostFormat("User = %s login success.", UserName.c_str()));
    evhttp_send_error(Request, HTTP_OK, "Login success.");
    return true;
}

bool UserHandler::ProcessLogout(evhttp_request * Request)
{
    return false;
}
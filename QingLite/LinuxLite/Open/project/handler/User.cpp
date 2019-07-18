#include "User.h"
#include <event.h>
#include <event2/http.h>
#include "DatabaseManager.h"
#include "../core/tools/BoostLog.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>



User::User()
{
}

User::~User()
{
}

bool User::ProcessLogin(evhttp_request * Request)
{
    struct evbuffer* PostData = evhttp_request_get_input_buffer(Request);
    if (PostData == NULL)
    {
        BoostLog::WriteError("User Login: post data is NULL.");
        return false;
    }

    size_t PostDataSize = evbuffer_get_length(PostData);
    if (PostDataSize <= 0)
    {
        BoostLog::WriteError("User Login: post data size is 0.");
        return false;
    }

    std::vector<char> PostDataBuffer(PostDataSize + 1, 0);
    int ReadSize = evbuffer_remove(PostData, &PostDataBuffer[0], PostDataSize);
    if (ReadSize != static_cast<int>(PostDataSize))
    {
        BoostLog::WriteError(BoostFormat("User Login: data size = %d, read size = %d.", PostDataSize, ReadSize));
        return false;
    }

    BoostLog::WriteDebug(BoostFormat("Post data = %s, size = %d.", &PostDataBuffer[0], PostDataBuffer.size()));

    std::stringstream JsonString(std::string(PostDataBuffer.begin(), PostDataBuffer.end() - 1));
    boost::property_tree::ptree JsonTree;
    boost::property_tree::read_json(JsonString, JsonTree);

    const std::string &UserName = JsonTree.get<std::string>("username");
    const std::string &Password = JsonTree.get<std::string>("password");
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

bool User::ProcessLogout(evhttp_request * Request)
{
    return false;
}
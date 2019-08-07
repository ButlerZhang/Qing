#include "UserHandler.h"
#include "../../../../../Common/Boost/BoostLog.h"
#include "../../controller/HTTPServer.h"
#include "../../../../../Common/Database/MySQLDatabase.h"
#include "../../core/network/ThreadNoticeQueue.h"
#include <event.h>
#include <event2/http.h>



UserHandler::UserHandler()
{
}

UserHandler::~UserHandler()
{
}

bool UserHandler::ProcessLogin(evhttp_request *Request)
{
    boost::property_tree::ptree JsonTree;
    if (!ParsePostData(JsonTree, GetPostDataString(Request)))
    {
        return false;
    }

    const std::string &UserName = JsonTree.get<std::string>("Name");
    const std::string &Password = JsonTree.get<std::string>("Password");
    g_Log.WriteDebug(BoostFormat("Process login: User name = %s, Password = %s", UserName.c_str(), Password.c_str()));

    UserModel ReplyModel;
    ReplyModel.m_ErrorCode = 0;
    ReplyModel.m_UserName = UserName;
    ReplyModel.m_ReplayMessage = "Login successful!";

    MySQLDataSet DataSet;
    std::string SQLString(BoostFormat("SELECT * FROM user WHERE name = '%s'", UserName.c_str()));
    if (!g_HTTPServer.GetDB().ExecuteQuery(SQLString.c_str(), &DataSet))
    {
        ReplyModel.m_ErrorCode = 105;
        ReplyModel.m_ReplayMessage = "Login failed, query database error!";
        g_Log.WriteDebug(BoostFormat("Process login: Execute query failed: %s", SQLString.c_str()));
        return SendLoginReply(Request, JsonTree, ReplyModel);
    }

    if (DataSet.GetRecordCount() <= 0)
    {
        ReplyModel.m_ErrorCode = 101;
        ReplyModel.m_ReplayMessage = "Login failed, user does not exist!";
        g_Log.WriteDebug(BoostFormat("Process login: Can not find user = %s", UserName.c_str()));
        return SendLoginReply(Request, JsonTree, ReplyModel);
    }

    int IsLock = 0;
    std::string DBPassword, CreateTime, LastUpdateTime;
    if(!DataSet.GetValue("password", DBPassword) ||
       !DataSet.GetValue("create_time", CreateTime) ||
       !DataSet.GetValue("last_update_time", LastUpdateTime) ||
       !DataSet.GetValue("is_lock", IsLock) ||
        !DataSet.GetValue("authority_id", ReplyModel.m_AuthorityID))
    {
        ReplyModel.m_ErrorCode = 105;
        ReplyModel.m_ReplayMessage = "Login failed, get database data error!";
        g_Log.WriteDebug("Process login: Get user value falied.");
        return SendLoginReply(Request, JsonTree, ReplyModel);
    }

    if (Password != DBPassword)
    {
        ReplyModel.m_ErrorCode = 104;
        ReplyModel.m_ReplayMessage = "Login failed, the password is inocrrect!";
        g_Log.WriteDebug("Process login: Password does not match");
        return SendLoginReply(Request, JsonTree, ReplyModel);
    }

    if (CreateTime == LastUpdateTime)
    {
        ReplyModel.m_ErrorCode = 102;
        ReplyModel.m_ReplayMessage = "Login failed, please change your password for the first login!";
        g_Log.WriteDebug("Process login: " + ReplyModel.m_ReplayMessage);
        return SendLoginReply(Request, JsonTree, ReplyModel);
    }

    if (IsLock)
    {
        ReplyModel.m_ErrorCode = 103;
        ReplyModel.m_ReplayMessage = "Login failed, user is locked, please contact the administrators!";
        g_Log.WriteDebug("Process login: " + ReplyModel.m_ReplayMessage);
        return SendLoginReply(Request, JsonTree, ReplyModel);
    }

    g_Log.WriteDebug(BoostFormat("Process login: User = %s login success.", UserName.c_str()));
    return SendLoginReply(Request, JsonTree, ReplyModel);
}

bool UserHandler::ProcessLogout(evhttp_request * Request)
{
    return false;
}

bool UserHandler::SendLoginReply(struct evhttp_request *Request, boost::property_tree::ptree &JsonTree, const UserModel &ReplyModel)
{
    JsonTree.clear();
    JsonTree.put("ErrorCode", ReplyModel.m_ErrorCode);
    JsonTree.put("Message", ReplyModel.m_ReplayMessage);

    boost::property_tree::ptree UserTree;
    UserTree.put("AuthorityID", ReplyModel.m_AuthorityID);
    UserTree.put("Name", ReplyModel.m_UserName);
    JsonTree.push_back(std::make_pair("UserInfo", UserTree));

    const std::string &JsonString = GetReplyJsonString(JsonTree);

    if (ReplyModel.m_ErrorCode == 0)
    {
        const std::string &SessionID = g_HTTPServer.GetHTTPSession().GenerateSession(ReplyModel.m_UserName, ReplyModel.m_Password);
        const std::string &Cookie = BoostFormat("SID=%s; Max-Age=3600; HttpOnly", SessionID.c_str());
        evhttp_add_header(evhttp_request_get_output_headers(Request), "Set-Cookie", Cookie.c_str());
    }

    EventDataBuffer DataBuffer;
    evbuffer_add_printf(DataBuffer.m_evbuffer, JsonString.c_str());
    evhttp_send_reply(Request, ReplyModel.m_ErrorCode, ReplyModel.m_ReplayMessage.c_str(), DataBuffer.m_evbuffer);

    return g_ThreadNoticeQueue.PushMessage(JsonString);
}

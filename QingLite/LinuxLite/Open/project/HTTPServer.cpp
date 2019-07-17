#include "HTTPServer.h"
#include "Tools/BoostLog.h"
#include "../../LinuxTools.h"



HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{
}

bool HTTPServer::ProcessGet(evhttp_request *Request)
{
    return HTTPBaseServer::ProcessGet(Request);
}

bool HTTPServer::ProcessPost(evhttp_request * Request)
{
    std::vector<std::string> StringVector;
    std::string RequestURI(evhttp_request_get_uri(Request));
    if (!SplitString(RequestURI, StringVector, "/"))
    {
        BoostLog::WriteError("Process Post: Split string failed.");
        return HTTPBaseServer::ProcessPost(Request);
    }

    std::string LogString("String vector size = " + std::to_string(StringVector.size()) + ": ");
    for (std::vector<std::string>::size_type Index = 0; Index < StringVector.size(); Index++)
    {
        LogString += std::to_string(Index) + "=" + StringVector[Index] + ";";
    }
    LogString.erase(LogString.end() - 1);
    BoostLog::WriteDebug(LogString);

    if (StringVector[0] == "user")
    {
        if (StringVector[1] == "login")
        {
            return ProcessUserLogin(Request);
        }
        else if (StringVector[1] == "logout")
        {
            return ProcessUserLogout(Request);
        }
    }
    else
    {

    }

    return HTTPBaseServer::ProcessPost(Request);
}

bool HTTPServer::ProcessUserLogin(evhttp_request * Request)
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
    return HTTPBaseServer::ProcessPost(Request);
}

bool HTTPServer::ProcessUserLogout(evhttp_request * Request)
{
    return false;
}

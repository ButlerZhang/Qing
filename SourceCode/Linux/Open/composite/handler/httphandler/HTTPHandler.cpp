#include "HTTPHandler.h"
#include "../../../../../Common/Boost/BoostLog.h"
#include <event.h>
#include <event2/http.h>



HTTPHandler::HTTPHandler()
{
}

HTTPHandler::~HTTPHandler()
{
}

std::string HTTPHandler::GetPostDataString(evhttp_request * Request)
{
    std::string PostDataString;

    struct evbuffer* PostData = evhttp_request_get_input_buffer(Request);
    if (PostData == NULL)
    {
        g_Log.WriteError("Handler: post data is NULL.");
        return PostDataString;
    }

    size_t PostDataSize = evbuffer_get_length(PostData);
    if (PostDataSize <= 0)
    {
        g_Log.WriteError("Handler: post data size is 0.");
        return PostDataString;
    }

    std::vector<char> PostDataBuffer(PostDataSize + 1, 0);
    int ReadSize = evbuffer_remove(PostData, &PostDataBuffer[0], PostDataSize);
    if (ReadSize != static_cast<int>(PostDataSize))
    {
        g_Log.WriteError(BoostFormat("Handler: data size = %d, read size = %d.", PostDataSize, ReadSize));
        return PostDataString;
    }

    g_Log.WriteDebug(BoostFormat("Post data = %s, size = %d.", &PostDataBuffer[0], PostDataBuffer.size()));
    PostDataString.assign(PostDataBuffer.begin(), PostDataBuffer.end() - 1);
    return PostDataString;
}

std::string HTTPHandler::GetReplyJsonString(boost::property_tree::ptree &JsonTree)
{
    std::string JsonString;
    try
    {
        std::stringstream JsonStream;
        boost::property_tree::write_json(JsonStream, JsonTree, false);
        JsonString = JsonStream.str();
        JsonString.erase(JsonString.end() - 1);
        g_Log.WriteDebug(BoostFormat("Handler: reply json = %s", JsonString.c_str()));
    }
    catch (...)
    {
        g_Log.WriteError("Handler: Write json tree error.");
    }
    return JsonString;
}

bool HTTPHandler::ParsePostData(boost::property_tree::ptree &JsonTree, const std::string &PostDataString)
{
    if (PostDataString.empty())
    {
        g_Log.WriteError("Handler: post data string is empty.");
        return false;
    }

    try
    {
        std::stringstream JsonString(PostDataString);
        boost::property_tree::read_json(JsonString, JsonTree);
    }
    catch (...)
    {
        g_Log.WriteError("Handler: Read json tree error.");
        return false;
    }

    return true;
}

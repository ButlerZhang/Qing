#include "Handler.h"
#include "../core/tools/BoostLog.h"
#include <event.h>
#include <event2/http.h>



Handler::Handler()
{
    m_evbuffer = evbuffer_new();
}

Handler::~Handler()
{
    evbuffer_free(m_evbuffer);
}

std::string Handler::GetReplyJsonString()
{
    std::string JsonString;

    try
    {
        std::stringstream JsonStream;
        boost::property_tree::write_json(JsonStream, m_JsonTree, false);

        JsonString = JsonStream.str();
        JsonString.erase(JsonString.end() - 1);
        BoostLog::WriteDebug(BoostFormat("Handler: reply json = %s", JsonString.c_str()));
    }
    catch (...)
    {
        BoostLog::WriteError("Handler: Write json tree error.");
    }

    return JsonString;
}

std::string Handler::GetPostDataString(evhttp_request * Request)
{
    std::string PostDataString;

    struct evbuffer* PostData = evhttp_request_get_input_buffer(Request);
    if (PostData == NULL)
    {
        BoostLog::WriteError("Handler: post data is NULL.");
        return PostDataString;
    }

    size_t PostDataSize = evbuffer_get_length(PostData);
    if (PostDataSize <= 0)
    {
        BoostLog::WriteError("Handler: post data size is 0.");
        return PostDataString;
    }

    std::vector<char> PostDataBuffer(PostDataSize + 1, 0);
    int ReadSize = evbuffer_remove(PostData, &PostDataBuffer[0], PostDataSize);
    if (ReadSize != static_cast<int>(PostDataSize))
    {
        BoostLog::WriteError(BoostFormat("Handler: data size = %d, read size = %d.", PostDataSize, ReadSize));
        return PostDataString;
    }

    BoostLog::WriteDebug(BoostFormat("Post data = %s, size = %d.", &PostDataBuffer[0], PostDataBuffer.size()));
    PostDataString.assign(PostDataBuffer.begin(), PostDataBuffer.end() - 1);
    return PostDataString;
}

bool Handler::ParsePostData(const std::string &PostDataString)
{
    if (PostDataString.empty())
    {
        BoostLog::WriteError("Handler: post data string is empty.");
        return false;
    }

    try
    {
        m_JsonTree.clear();
        std::stringstream JsonString(PostDataString);
        boost::property_tree::read_json(JsonString, m_JsonTree);
    }
    catch (...)
    {
        BoostLog::WriteError("Handler: Read json tree error.");
        return false;
    }

    return true;
}

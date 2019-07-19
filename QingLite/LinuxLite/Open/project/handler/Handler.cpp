#include "Handler.h"
#include "../core/tools/BoostLog.h"
#include <event.h>
#include <event2/http.h>



Handler::Handler()
{
}

Handler::~Handler()
{
}

std::string Handler::GetPostData(evhttp_request * Request)
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

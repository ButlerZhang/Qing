#include "HTTPServer.h"
#include "../core/tools/BoostLog.h"
#include "../../../LinuxTools.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>



HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{
}

bool HTTPServer::ProcessGet(evhttp_request *Request)
{
    std::string RequestPath;
    GetRequestPath(Request, RequestPath);

    if (RequestPath == "/")
    {
        RequestPath = "./jpc-web/index.html";
    }
    else
    {
        std::vector<std::string> PathVector;
        if (!SplitRequestPath(RequestPath, PathVector))
        {
            return HTTPBaseServer::ProcessGet(Request);
        }

        std::string WorkPath;
        if (PathVector[0] == "log")
        {
            WorkPath = "./LogFile/";
            if (PathVector.size() == 1)
            {
                RequestPath = WorkPath;
            }
            else
            {
                RequestPath.replace(0, PathVector[0].size(), WorkPath);
            }
        }
        else
        {
            WorkPath = "./jpc-web";
            if (PathVector.size() == 1)
            {
                RequestPath.insert(0, WorkPath);
            }
            else
            {
                RequestPath.replace(0, PathVector[0].size(), WorkPath);
            }
        }
    }

    struct stat ActuallyPathStat;
    if (stat(RequestPath.c_str(), &ActuallyPathStat) < 0)
    {
        BoostLog::WriteError(BoostFormat("Stat path = %s failed.", RequestPath.c_str()));
        return false;
    }

    if (S_ISDIR(ActuallyPathStat.st_mode))
    {
        return ProcessDirectory(Request, RequestPath);
    }
    else
    {
        return ProcessFile(Request, ActuallyPathStat, RequestPath);
    }
}

bool HTTPServer::ProcessPost(evhttp_request *Request)
{
    std::string RequestPath;
    GetRequestPath(Request, RequestPath);

    std::vector<std::string> PathVector;
    if (!SplitRequestPath(RequestPath, PathVector) || PathVector.empty())
    {
        return HTTPBaseServer::ProcessPost(Request);
    }

    if (PathVector[0] == "user")
    {
        if (PathVector[1] == "login")
        {
            return ProcessUserLogin(Request);
        }
        else if (PathVector[1] == "logout")
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

    std::stringstream JsonString(std::string(PostDataBuffer.begin(), PostDataBuffer.end() - 1));
    boost::property_tree::ptree JsonTree;
    boost::property_tree::read_json(JsonString, JsonTree);

    const std::string &UserName = JsonTree.get<std::string>("username");
    const std::string &Password = JsonTree.get<std::string>("password");
    BoostLog::WriteDebug(BoostFormat("User name = %s, Password = %s", UserName.c_str(), Password.c_str()));

    return HTTPBaseServer::ProcessPost(Request);
}

bool HTTPServer::ProcessUserLogout(evhttp_request * Request)
{
    return false;
}

bool HTTPServer::GetRequestPath(evhttp_request *Request, std::string &RequestPath)
{
    const char *RefererArray = evhttp_find_header(evhttp_request_get_input_headers(Request), "Referer");
    if (RefererArray != NULL)
    {
        std::string Header("http://");
        std::string Referer(RefererArray);
        std::string::size_type StartIndex = Referer.find_first_of("/", Header.size()) + 1;
        if (StartIndex != std::string::npos)
        {
            RequestPath.append(Referer.substr(StartIndex));
        }
    }

    RequestPath.append(evhttp_request_get_uri(Request));
    BoostLog::WriteDebug("Request path = " + RequestPath);
    return true;
}

bool HTTPServer::SplitRequestPath(const std::string &RequestPath, std::vector<std::string> &PathVector)
{
    const std::string Seperator("/");
    if (RequestPath == Seperator)
    {
        BoostLog::WriteDebug("Split request URI, URI = /");
        return true;
    }

    if (!SplitString(RequestPath, PathVector, Seperator))
    {
        BoostLog::WriteError("Split request URI failed.");
        return false;
    }

    std::string LogString("String vector size = " + std::to_string(PathVector.size()) + ": ");
    for (std::vector<std::string>::size_type Index = 0; Index < PathVector.size(); Index++)
    {
        LogString += std::to_string(Index) + "=" + PathVector[Index] + "; ";
    }

    LogString.erase(LogString.end() - 2, LogString.end());
    BoostLog::WriteDebug(LogString);
    return true;
}

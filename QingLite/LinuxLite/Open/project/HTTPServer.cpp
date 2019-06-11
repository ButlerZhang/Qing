#include "HTTPServer.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>



HTTPServer::HTTPServer()
{
    m_evHTTP = NULL;
    m_EventBase = NULL;

    m_ContentTypeMap["txt"] = "text/plain";
    m_ContentTypeMap["c"] = "text/plain";
    m_ContentTypeMap["h"] = "text/plain";
    m_ContentTypeMap["html"] = "text/html";
    m_ContentTypeMap["htm"] = "text/htm";
    m_ContentTypeMap["css"] = "text/css";
    m_ContentTypeMap["gif"] = "image/gif";
    m_ContentTypeMap["jpg"] = "image/jpeg";
    m_ContentTypeMap["jpeg"] = "image/jpeg";
    m_ContentTypeMap["png"] = "image/png";
    m_ContentTypeMap["ogg"] = "application/ogg";
    m_ContentTypeMap["pdf"] = "application/pdf";
    m_ContentTypeMap["ps"] = "application/postscript";
    m_ContentTypeMap["au"] = "audio/basic";
    m_ContentTypeMap["wav"] = "audio/wav";
    m_ContentTypeMap["mid"] = "audio/midi";
    m_ContentTypeMap["midi"] = "audio/midi";
    m_ContentTypeMap["mp3"] = "audio/mpeg";
    m_ContentTypeMap["avi"] = "video/x-msvideo";
    m_ContentTypeMap["qt"] = "video/quicktime";
    m_ContentTypeMap["mov"] = "video/quicktime";
    m_ContentTypeMap["mpeg"] = "video/mpeg";
    m_ContentTypeMap["mpe"] = "video/mpeg";
    m_ContentTypeMap["vrml"] = "model/vrml";
    m_ContentTypeMap["misc"] = "application/misc";
}

HTTPServer::~HTTPServer()
{
    m_EventBase = NULL;
    m_ContentTypeMap.clear();

    if (m_evHTTP != NULL)
    {
        evhttp_free(m_evHTTP);
        m_evHTTP = NULL;
    }
}

bool HTTPServer::BindEventBase(event_base * EventBase)
{
    m_EventBase = EventBase;
    return m_EventBase != NULL;
}

bool HTTPServer::Start(const std::string & ServerIP, int Port)
{
    if (m_EventBase == NULL)
    {
        printf("ERROR: No binding event base.\n");
        return false;
    }

    if (m_evHTTP != NULL)
    {
        printf("ERROR: re-start.\n");
        return true;
    }

    m_evHTTP = evhttp_new(m_EventBase);
    if (m_evHTTP == NULL)
    {
        printf("ERROR: could not create evhttp.\n");
        return false;
    }

    if (evhttp_bind_socket(m_evHTTP, ServerIP.c_str(), static_cast<uint16_t>(Port)) != 0)
    {
        printf("ERROR: http server bind failed.\n");
        return false;
    }

    evhttp_set_timeout(m_evHTTP, 5);
    evhttp_set_gencb(m_evHTTP, CallBack_GenericRequest, this);

    return true;
}

void HTTPServer::PrintRequest(evhttp_request * Request)
{
    const char *RequestType = NULL;
    switch (evhttp_request_get_command(Request))
    {
    case EVHTTP_REQ_GET:        RequestType = "GET";        break;
    case EVHTTP_REQ_POST:       RequestType = "POST";       break;
    case EVHTTP_REQ_HEAD:       RequestType = "HEAD";       break;
    case EVHTTP_REQ_PUT:        RequestType = "PUT";        break;
    case EVHTTP_REQ_DELETE:     RequestType = "DELETE";     break;
    case EVHTTP_REQ_OPTIONS:    RequestType = "OPTIONS";    break;
    case EVHTTP_REQ_TRACE:      RequestType = "TRACE";      break;
    case EVHTTP_REQ_CONNECT:    RequestType = "CONNECT";    break;
    case EVHTTP_REQ_PATCH:      RequestType = "PATCH";      break;
    default:                    RequestType = "unknown";    break;
    }

    printf("==========new request=========\n");
    printf("Received a %s request for: %s\nHeaders:\n", RequestType, evhttp_request_get_uri(Request));

    struct evkeyvalq *Headers = evhttp_request_get_input_headers(Request);
    for (struct evkeyval *header = Headers->tqh_first; header != NULL; header = header->next.tqe_next)
    {
        printf("\t%s: %s\n", header->key, header->value);
    }

    printf("\r\n");
}

bool HTTPServer::ParseRequestPath(evhttp_request * Request, std::string & ActualllyPath)
{
    const char *OriginalURI = evhttp_request_get_uri(Request);
    printf("Original URI = %s\n", OriginalURI);

    struct evhttp_uri *ParseURI = evhttp_uri_parse(OriginalURI);
    if (ParseURI == NULL)
    {
        evhttp_send_error(Request, HTTP_BADREQUEST, "Can not decoded URI.");
        printf("It is not a good URI. Sending BADREQUEST.\n");
        return false;
    }

    printf("scheme:%s\n", evhttp_uri_get_scheme(ParseURI));
    printf("host:%s\n", evhttp_uri_get_host(ParseURI));
    printf("path:%s\n", evhttp_uri_get_path(ParseURI));
    printf("port:%d\n", evhttp_uri_get_port(ParseURI));
    printf("query:%s\n", evhttp_uri_get_query(ParseURI));
    printf("userinfo:%s\n", evhttp_uri_get_userinfo(ParseURI));
    printf("fragment:%s\n\n", evhttp_uri_get_fragment(ParseURI));

    const char *RequestPath = evhttp_uri_get_path(ParseURI);
    printf("Request path = %s\n", RequestPath);
    if (RequestPath == NULL)
    {
        RequestPath = "/";
    }

    char *DecodeURI = evhttp_uridecode(RequestPath, 0, NULL);
    if (DecodeURI == NULL)
    {
        evhttp_send_error(Request, HTTP_NOTFOUND, "Document was not found.");
        printf("Decoded URI is NULL.\n");
        evhttp_uri_free(ParseURI);
        return false;
    }

    if (strstr(DecodeURI, ".."))
    {
        evhttp_send_error(Request, HTTP_NOTFOUND, "Document was not found.");
        printf("Decoded URI include '..' directory.\n");
        evhttp_uri_free(ParseURI);
        free(DecodeURI);
        return false;
    }

    if (strcmp(DecodeURI, "/") == 0 || strcmp(DecodeURI, "") == 0)
    {
        ActualllyPath = "./";
    }
    else
    {
        ActualllyPath.append(".");
        ActualllyPath.append(DecodeURI);
    }

    printf("Actuallly path = %s\r\n", ActualllyPath.c_str());
    evhttp_uri_free(ParseURI);
    free(DecodeURI);
    return true;
}

void HTTPServer::ProcessDirectory(evhttp_request * Request, const std::string & ActualllyPath)
{
    struct evbuffer *evb = evbuffer_new();
    if (evb == NULL)
    {
        printf("ERROR: evbuffer create failed.\n");
        return;
    }

    evbuffer_add_printf(evb,
        "<!DOCTYPE html>\n"
        "<html>\n <head>\n"
        "  <meta charset='utf-8'>\n"
        "  <title>Qing Server</title>\n"
        "  <base href='Directory'>\n"
        " </head>\n"
        " <body>\n"
        "  <h1>Floder:</h1>\n"
        "  <ul>\n");

    printf("\nFloder:\r\n");

    struct stat FileStat;
    struct dirent **DirentInfo;
    int FileCount = scandir(ActualllyPath.c_str(), &DirentInfo, NULL, alphasort);
    for (int FileIndex = 0; FileIndex < FileCount; FileIndex++)
    {
        const char *FileName = DirentInfo[FileIndex]->d_name;
        if (strcmp(FileName, ".") == 0)
        {
            continue;
        }

        printf("\t%s\n", FileName);
        if (lstat((ActualllyPath + FileName).c_str(), &FileStat) < 0)
        {
            evbuffer_add_printf(evb, "   <li><a href=\"%s\">%s</a>\n", FileName, FileName);
        }
        else
        {
            if (S_ISDIR(FileStat.st_mode))
            {
                evbuffer_add_printf(evb, "   <li><a href=\"%s/\">%s/</a>\n", FileName, FileName);
            }
            else
            {
                evbuffer_add_printf(evb, "   <li><a href=\"%s\">%s</a>\n", FileName, FileName);
            }
        }
    }

    printf("\r\n");
    evbuffer_add_printf(evb, "</ul></body></html>\n");
    evhttp_add_header(evhttp_request_get_output_headers(Request), "Content-Type", "text/html");

    evhttp_send_reply(Request, HTTP_OK, "OK", evb);
    evbuffer_free(evb);
}

void HTTPServer::ProcessFile(evhttp_request * Request, struct stat & FileStat, const std::string & ActualllyPath)
{
    int FileDescriptor = open(ActualllyPath.c_str(), O_RDONLY);
    if (FileDescriptor <= 0)
    {
        evhttp_send_error(Request, HTTP_NOTFOUND, "File was not found.");
        printf("ERROR: Open file descriptor failed.\n");
        return;
    }

    if (fstat(FileDescriptor, &FileStat) < 0)
    {
        printf("ERROR: fstat failed.\n");
        close(FileDescriptor);
        return;
    }

    struct evbuffer *evb = evbuffer_new();
    if (evb == NULL)
    {
        printf("ERROR: evbuffer create failed.\n");
        close(FileDescriptor);
        return;
    }

    const char *FileExtension = strrchr(ActualllyPath.c_str(), '.') + 1;
    std::map<std::string, std::string>::iterator it = m_ContentTypeMap.find(FileExtension);
    const char *FileType = (it != m_ContentTypeMap.end()) ? it->second.c_str() : m_ContentTypeMap["misc"].c_str();
    printf("\r\nFile:\r\n\tExtension = %s\n\tType = %s\n\n", FileExtension, FileType);

    evhttp_add_header(evhttp_request_get_output_headers(Request), "Content-Type", FileType);
    evbuffer_add_file(evb, FileDescriptor, 0, FileStat.st_size);
    evhttp_send_reply(Request, HTTP_OK, "OK", evb);

    close(FileDescriptor);
    evbuffer_free(evb);
}

void HTTPServer::CallBack_GenericRequest(evhttp_request * Request, void * arg)
{
    HTTPServer *Server = (HTTPServer*)arg;
    Server->PrintRequest(Request);

    std::string FullPath;
    if (!Server->ParseRequestPath(Request, FullPath))
    {
        return;
    }

    evhttp_cmd_type CurrentType = evhttp_request_get_command(Request);
    if (CurrentType == EVHTTP_REQ_GET)
    {
        struct stat ActuallyPathStat;
        if (stat(FullPath.c_str(), &ActuallyPathStat) < 0)
        {
            printf("ERROR: Stat actually path failed.\n\n");
            return;
        }

        if (S_ISDIR(ActuallyPathStat.st_mode))
        {
            Server->ProcessDirectory(Request, FullPath);
        }
        else
        {
            Server->ProcessFile(Request, ActuallyPathStat, FullPath);
        }
    }
    else if (CurrentType == EVHTTP_REQ_POST)
    {
        evhttp_send_error(Request, HTTP_NOTIMPLEMENTED, "Post not implemented.");
        struct evbuffer* PostData = evhttp_request_get_input_buffer(Request);

        char PostDataBuffer[1024];
        memset(PostDataBuffer, 0, sizeof(PostDataBuffer));
        int ReadSize = evbuffer_remove(PostData, PostDataBuffer, sizeof(PostDataBuffer));
        printf("Post data = %s, size = %d.\n\n", PostDataBuffer, ReadSize);
    }
    else
    {
        evhttp_send_error(Request, HTTP_BADMETHOD, "method not allowed for this uri.");
        printf("method not allowed for this uri.\n\n");
    }
}
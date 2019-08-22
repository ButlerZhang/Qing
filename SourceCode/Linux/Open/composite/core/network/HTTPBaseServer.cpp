#include "HTTPBaseServer.h"
#include "../../../../../Common/Boost/BoostLog.h"
#include "../tools/OpenSSLContext.h"
#include <fcntl.h>
#include <dirent.h>
#include <event2/bufferevent_ssl.h>



HTTPBaseServer::HTTPBaseServer()
{
    m_ContentTypeMap["txt"] = "text/plain";
    m_ContentTypeMap["log"] = "text/plain";
    m_ContentTypeMap["c"] = "text/plain";
    m_ContentTypeMap["h"] = "text/plain";
    m_ContentTypeMap["html"] = "text/html";
    m_ContentTypeMap["htm"] = "text/htm";
    m_ContentTypeMap["svg"] = "text/xml";
    m_ContentTypeMap["css"] = "text/css";
    m_ContentTypeMap["gif"] = "image/gif";
    m_ContentTypeMap["jpg"] = "image/jpeg";
    m_ContentTypeMap["jpeg"] = "image/jpeg";
    m_ContentTypeMap["png"] = "image/png";
    m_ContentTypeMap["ogg"] = "application/ogg";
    m_ContentTypeMap["pdf"] = "application/pdf";
    m_ContentTypeMap["ps"] = "application/postscript";
    m_ContentTypeMap["js"] = "application/x-javascript";
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

HTTPBaseServer::~HTTPBaseServer()
{
    m_ContentTypeMap.clear();
    event_base_loopbreak(m_EventBase.m_eventbase);
    g_Log.WriteDebug("HTTP base server was destructored.");
}

bool HTTPBaseServer::Start(const std::string &ServerIP, int Port, bool IsEnableHTTPS)
{
    if (m_EventBase.m_eventbase == NULL)
    {
        g_Log.WriteError("HTTP base server event base is NULL.");
        return false;
    }

    if (!AddCheckoutTimer(5))
    {
        g_Log.WriteError("HTTP base server add checkout timer failed.");
        return false;
    }

    m_EventHTTP.m_evhttp = evhttp_new(m_EventBase.m_eventbase);
    if (m_EventHTTP.m_evhttp == NULL)
    {
        g_Log.WriteError("HTTP base server could not create evhttp.");
        return false;
    }

    if (evhttp_bind_socket(m_EventHTTP.m_evhttp, ServerIP.c_str(), static_cast<uint16_t>(Port)) != 0)
    {
        g_Log.WriteError(BoostFormat("HTTP base server bind (%s,%d) failed.", ServerIP.c_str(), Port));
        return false;
    }

    if (IsEnableHTTPS)
    {
        static OpenSSLContext SSL;
        SSL_CTX *SSLContext = SSL.CreateSSLContext("./server/server-cert.pem", "./server/server-key.pem", "./ca/ca-cert.pem");
        if (SSLContext == NULL)
        {
            g_Log.WriteError("HTTP base server create SSL context failed.");
            return false;
        }
        evhttp_set_bevcb(m_EventHTTP.m_evhttp, CallBack_CreateSSLBufferevent, SSLContext);
    }

    evhttp_set_timeout(m_EventHTTP.m_evhttp, 5);
    evhttp_set_gencb(m_EventHTTP.m_evhttp, CallBack_GenericRequest, this);

    g_Log.WriteInfo(BoostFormat("HTTP Server(%s:%d) start dispatch...", ServerIP.c_str(), Port));
    event_base_dispatch(m_EventBase.m_eventbase);
    return true;
}

bool HTTPBaseServer::ProcessRequest(evhttp_request *Request)
{
    PrintRequest(Request);

    switch (evhttp_request_get_command(Request))
    {
    case EVHTTP_REQ_GET:    return ProcessGet(Request);
    case EVHTTP_REQ_POST:   return ProcessPost(Request);
    default:                break;
    }

    evhttp_send_error(Request, HTTP_BADMETHOD, "method not allowed for this uri.");
    g_Log.WriteError("HTTP base server: method not allowed for this uri.");
    return true;
}

bool HTTPBaseServer::ProcessGet(evhttp_request *Request)
{
    std::string FullPath;
    if (!ParseRequestPath(Request, FullPath))
    {
        return false;
    }

    struct stat ActuallyPathStat;
    if (stat(FullPath.c_str(), &ActuallyPathStat) < 0)
    {
        g_Log.WriteError(BoostFormat("HTTP base server: stat path = %s failed.", FullPath.c_str()));
        evhttp_send_error(Request, HTTP_BADMETHOD, "File was not found.");
        return false;
    }

    if (S_ISDIR(ActuallyPathStat.st_mode))
    {
        return ProcessDirectory(Request, FullPath);
    }
    else
    {
        return ProcessFile(Request, ActuallyPathStat, FullPath);
    }
}

bool HTTPBaseServer::ProcessPost(evhttp_request * Request)
{
    evhttp_send_error(Request, HTTP_NOTIMPLEMENTED, "Post not implemented.");
    g_Log.WriteError("HTTP base server: Post not implemented.");
    return false;
}

bool HTTPBaseServer::AddCheckoutTimer(int TimerInternal)
{
    if (m_CheckoutTimer.m_event != NULL)
    {
        g_Log.WriteError("HTTP base server re-create checkout timer.");
        return true;
    }

    m_CheckoutTimer.m_event = event_new(m_EventBase.m_eventbase, -1, EV_PERSIST, CallBack_Checkout, this);
    if (m_CheckoutTimer.m_event == NULL)
    {
        g_Log.WriteError("HTTP base server create chekcout timer failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = TimerInternal;

    if (event_add(m_CheckoutTimer.m_event, &tv) != 0)
    {
        g_Log.WriteError("HTTP base server add checkout timer failed.");
        return false;
    }

    return true;
}

void HTTPBaseServer::PrintHeaders(evkeyvalq *Headers, bool IsRequest)
{
    std::string LogString(BoostFormat("%s headers:\n", IsRequest ? "Request" : "Response"));
    for (struct evkeyval *header = Headers->tqh_first; header != NULL; header = header->next.tqe_next)
    {
        LogString.append(BoostFormat("\t%s: %s\n", header->key, header->value));
    }
    LogString.erase(LogString.end() - 1);
    g_Log.WriteDebug(LogString);
}

void HTTPBaseServer::PrintRequest(evhttp_request *Request)
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

    g_Log.WriteDebug(BoostFormat("HTTP base server received a %s request for: %s", RequestType, evhttp_request_get_uri(Request)));
    PrintHeaders(evhttp_request_get_input_headers(Request), true);
}

std::string HTTPBaseServer::GetSessionID(evhttp_request *Request) const
{
    const char *Cookie = evhttp_find_header(evhttp_request_get_input_headers(Request), "Cookie");
    if (Cookie == NULL)
    {
        g_Log.WriteError("HTTP base server can not find cookie.");
        return std::string();
    }

    const std::string SIDFlag("SID=");
    const std::string CookieString(Cookie);
    std::string::size_type StatIndex = CookieString.find(SIDFlag);
    if (StatIndex == std::string::npos)
    {
        g_Log.WriteError("HTTP base server can not find SID in cookie.");
        return std::string();
    }

    std::string::size_type StopIndex = CookieString.size() - StatIndex - SIDFlag.size();
    const std::string &SessionID = CookieString.substr(StatIndex + SIDFlag.size(), StopIndex);
    if (!m_HTTPSession.IsSessionExisted(SessionID))
    {
        g_Log.WriteError(BoostFormat("HTTP base server sid = %s is not existed.", SessionID.c_str()));
        return std::string();
    }

    return SessionID;
}

bool HTTPBaseServer::GetRequestIPandPort(evhttp_connection *Connection, std::string &RequestIP, int &Port)
{
    if (Connection != NULL)
    {
        Port = 0;
        char IP[32] = { 0 };
        char *AddressPointer = IP;
        char **ArgumentPointer = &AddressPointer;
        evhttp_connection_get_peer(Connection, ArgumentPointer, (ev_uint16_t*)&Port);
        RequestIP = AddressPointer;
        return true;
    }

    return false;
}

bool HTTPBaseServer::ParseRequestPath(evhttp_request *Request, std::string &ActualllyPath)
{
    const char *OriginalURI = evhttp_request_get_uri(Request);
    struct evhttp_uri *ParseURI = evhttp_uri_parse(OriginalURI);
    if (ParseURI == NULL)
    {
        g_Log.WriteError(BoostFormat("HTTP base server: It is not a good URI = %s. Sending BADREQUEST.", OriginalURI));
        evhttp_send_error(Request, HTTP_BADREQUEST, "Can not parse original URI.");
        return false;
    }

    std::string LogString(BoostFormat("HTTP base server: Original URI = %s\n", OriginalURI));
    LogString.append(BoostFormat("\tscheme:%s\n", evhttp_uri_get_scheme(ParseURI)));
    LogString.append(BoostFormat("\thost:%s\n", evhttp_uri_get_host(ParseURI)));
    LogString.append(BoostFormat("\tpath:%s\n", evhttp_uri_get_path(ParseURI)));
    LogString.append(BoostFormat("\tport:%d\n", evhttp_uri_get_port(ParseURI)));
    LogString.append(BoostFormat("\tquery:%s\n", evhttp_uri_get_query(ParseURI)));
    LogString.append(BoostFormat("\tuserinfo:%s\n", evhttp_uri_get_userinfo(ParseURI)));
    LogString.append(BoostFormat("\tfragment:%s", evhttp_uri_get_fragment(ParseURI)));
    g_Log.WriteDebug(LogString);

    const char *RequestPath = evhttp_uri_get_path(ParseURI);
    if (RequestPath == NULL)
    {
        RequestPath = "/";
    }

    char *DecodeURI = evhttp_uridecode(RequestPath, 0, NULL);
    if (DecodeURI == NULL)
    {
        evhttp_send_error(Request, HTTP_NOTFOUND, "Document was not found.");
        g_Log.WriteError("HTTP base server: decoded URI is NULL.");
        evhttp_uri_free(ParseURI);
        return false;
    }

    if (strstr(DecodeURI, ".."))
    {
        evhttp_send_error(Request, HTTP_NOTFOUND, "Document was not found.");
        g_Log.WriteError("HTTP base server: decoded URI include '..' directory.");
        evhttp_uri_free(ParseURI);
        free(DecodeURI);
        return false;
    }

    if (strcmp(DecodeURI, "/") == 0 || strcmp(DecodeURI, "") == 0)
    {
        ActualllyPath.append("./");
    }
    else
    {
        ActualllyPath.append(".");
        ActualllyPath.append(DecodeURI);
    }

    g_Log.WriteDebug(BoostFormat("HTTP base server: Request actuallly path = %s", ActualllyPath.c_str()));
    evhttp_uri_free(ParseURI);
    free(DecodeURI);
    return true;
}

bool HTTPBaseServer::ProcessDirectory(evhttp_request *Request, const std::string &ActualllyPath)
{
    EventDataBuffer DataBuffer;
    evbuffer_add_printf(DataBuffer.m_evbuffer,
        "<!DOCTYPE html>\n"
        "<html>\n <head>\n"
        "  <meta charset='utf-8'>\n"
        "  <title>Qing Server</title>\n"
        "  <base href='Directory'>\n"
        " </head>\n"
        " <body>\n"
        "  <h1>Floder:</h1>\n"
        "  <ul>\n");

    struct stat FileStat;
    struct dirent **DirentInfo;
    std::string LogString("Directory:\n");

    int FileCount = scandir(ActualllyPath.c_str(), &DirentInfo, NULL, alphasort);
    for (int FileIndex = 0; FileIndex < FileCount; FileIndex++)
    {
        const char *FileName = DirentInfo[FileIndex]->d_name;
        if (strcmp(FileName, ".") == 0)
        {
            continue;
        }

        LogString.append(BoostFormat("\tFile= %s\n", FileName));
        if (lstat((ActualllyPath + FileName).c_str(), &FileStat) < 0)
        {
            evbuffer_add_printf(DataBuffer.m_evbuffer, "   <li><a href=\"%s\">%s</a>\n", FileName, FileName);
        }
        else
        {
            if (S_ISDIR(FileStat.st_mode))
            {
                evbuffer_add_printf(DataBuffer.m_evbuffer, "   <li><a href=\"%s/\">%s/</a>\n", FileName, FileName);
            }
            else
            {
                evbuffer_add_printf(DataBuffer.m_evbuffer, "   <li><a href=\"%s\">%s</a>\n", FileName, FileName);
            }
        }
    }

    evbuffer_add_printf(DataBuffer.m_evbuffer, "</ul></body></html>\n");
    size_t DataBufferLength = evbuffer_get_length(DataBuffer.m_evbuffer);

    struct evkeyvalq *ResponseHeader = evhttp_request_get_output_headers(Request);
    evhttp_add_header(ResponseHeader, "Content-Type", "text/html");
    evhttp_add_header(ResponseHeader, "Content-Length", std::to_string(DataBufferLength).c_str());

    evhttp_send_reply(Request, HTTP_OK, "OK", DataBuffer.m_evbuffer);
    PrintHeaders(ResponseHeader, false);
    g_Log.WriteDebug(LogString);
    return true;
}

bool HTTPBaseServer::ProcessFile(evhttp_request *Request, struct stat &FileStat, const std::string &ActualllyPath)
{
    int FileDescriptor = open(ActualllyPath.c_str(), O_RDONLY);
    if (FileDescriptor <= 0)
    {
        g_Log.WriteError("HTTP base server process file: open file descriptor failed.");
        evhttp_send_error(Request, HTTP_NOTFOUND, "File was not found.");
        return false;
    }

    g_Log.WriteDebug(BoostFormat("HTTP base server process file: path = %s, descriptor = %d.", ActualllyPath.c_str(), FileDescriptor));
    if (fstat(FileDescriptor, &FileStat) < 0)
    {
        g_Log.WriteError(BoostFormat("HTTP base server process file: file descriptor = %d fstat failed.", FileDescriptor));
        close(FileDescriptor);
        return false;
    }

    const char *FileExtension = strrchr(ActualllyPath.c_str(), '.') + 1;
    std::map<std::string, std::string>::iterator it = m_ContentTypeMap.find(FileExtension);
    const char *FileType = (it != m_ContentTypeMap.end()) ? it->second.c_str() : m_ContentTypeMap["misc"].c_str();

    struct evkeyvalq *ResponseHeader = evhttp_request_get_output_headers(Request);
    evhttp_add_header(ResponseHeader, "Content-Type", FileType);
    evhttp_add_header(ResponseHeader, "Content-Length", std::to_string(FileStat.st_size).c_str());

    EventDataBuffer DataBuffer;
    evbuffer_add_file(DataBuffer.m_evbuffer, FileDescriptor, 0, FileStat.st_size); //evbuffer_add_file will close file descriptor after succeed
    evhttp_send_reply(Request, HTTP_OK, "OK", DataBuffer.m_evbuffer);
    PrintHeaders(ResponseHeader, false);
    return true;
}

void HTTPBaseServer::CallBack_ConnectionClose(struct evhttp_connection *Connection, void *arg)
{
    int RequestPort = 0;
    std::string RequestIP;
    HTTPBaseServer *Server = (HTTPBaseServer*)arg;
    if (Server->GetRequestIPandPort(Connection, RequestIP, RequestPort))
    {
        g_Log.WriteDebug(BoostFormat("HTTP base server close connection, client ip = %s, port = %d", RequestIP.c_str(), RequestPort));
    }
    else
    {
        g_Log.WriteError("HTTP base server connection close unknow error.");
    }
}

void HTTPBaseServer::CallBack_GenericRequest(evhttp_request * Request, void * arg)
{
    int RequestPort = 0;
    std::string RequestIP;
    HTTPBaseServer *Server = (HTTPBaseServer*)arg;
    evhttp_connection *NewConnection = evhttp_request_get_connection(Request);
    evhttp_connection_set_closecb(NewConnection, CallBack_ConnectionClose, arg);
    Server->GetRequestIPandPort(NewConnection, RequestIP, RequestPort);
    g_Log.WriteDebug(BoostFormat("HTTP base server new request, client ip = %s, port = %d", RequestIP.c_str(), RequestPort));
    Server->ProcessRequest(Request);
}

void HTTPBaseServer::CallBack_Checkout(int Socket, short Events, void * UserData)
{
    HTTPBaseServer *Server = (HTTPBaseServer*)UserData;
    Server->ProcessCheckout();

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 5;          //TimerInternal
    event_add(Server->m_CheckoutTimer.m_event, &tv);
}

bufferevent* HTTPBaseServer::CallBack_CreateSSLBufferevent(event_base *base, void *arg)
{
    SSL_CTX *ctx = (SSL_CTX *)arg;
    struct bufferevent* SSLbev = bufferevent_openssl_socket_new(
        base,
        -1,
        SSL_new(ctx),
        BUFFEREVENT_SSL_ACCEPTING,
        BEV_OPT_CLOSE_ON_FREE);
    if (SSLbev == NULL)
    {
        g_Log.WriteError("HTTP base server create SSL bufferevent failed.");
    }
    return SSLbev;
}

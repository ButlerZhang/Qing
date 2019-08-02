#include "HTTPBaseServer.h"
#include "../../../../../Common/Boost/BoostLog.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <event2/bufferevent_ssl.h>



HTTPBaseServer::HTTPBaseServer()
{
    m_evHTTP = NULL;
    m_SSLContext = NULL;
    m_CheckoutTimer = NULL;
    m_EventBase = event_base_new();

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
    if (event_base_loopbreak(m_EventBase) != 0)
    {
        g_Log.WriteError("HTTP base server event can not loop break.");
    }

    if (m_evHTTP != NULL)
    {
        evhttp_free(m_evHTTP);
        m_evHTTP = NULL;
    }

    if (m_CheckoutTimer != NULL)
    {
        event_free(m_CheckoutTimer);
        m_CheckoutTimer = NULL;
    }

    if (m_SSLContext != NULL)
    {
        SSL_CTX_free(m_SSLContext);
        m_SSLContext = NULL;
    }
    event_base_free(m_EventBase);
    m_EventBase = NULL;
    g_Log.WriteDebug("HTTP base server was destructored.");
}

bool HTTPBaseServer::Start(const std::string &ServerIP, int Port, bool IsEnableHTTPS)
{
    if (m_EventBase == NULL)
    {
        g_Log.WriteError("HTTP base server event base is NULL.");
        return false;
    }

    if (!AddCheckoutTimer(5))
    {
        g_Log.WriteError("HTTP base server add checkout timer failed.");
        return false;
    }

    if (m_evHTTP != NULL)
    {
        g_Log.WriteError("HTTP base server re-start.");
        return true;
    }

    m_evHTTP = evhttp_new(m_EventBase);
    if (m_evHTTP == NULL)
    {
        g_Log.WriteError("HTTP base server could not create evhttp.");
        return false;
    }

    if (evhttp_bind_socket(m_evHTTP, ServerIP.c_str(), static_cast<uint16_t>(Port)) != 0)
    {
        g_Log.WriteError(BoostFormat("HTTP base server bind (%s,%d) failed.", ServerIP.c_str(), Port));
        return false;
    }

    if (IsEnableHTTPS)
    {
        m_SSLContext = CreateSSLContext("./server/server-cert.pem", "./server/server-key.pem", "./ca/ca-cert.pem");
        if (m_SSLContext == NULL)
        {
            g_Log.WriteError("HTTP base server create SSL context failed.");
            return false;
        }
        evhttp_set_bevcb(m_evHTTP, CallBack_Bufferevent, m_SSLContext);
    }
    //evhttp_set_timeout(m_evHTTP, 5);
    evhttp_set_gencb(m_evHTTP, CallBack_GenericRequest, this);

    g_Log.WriteInfo(BoostFormat("HTTP Server(%s:%d) start dispatch...", ServerIP.c_str(), Port));
    event_base_dispatch(m_EventBase);
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
    if (m_CheckoutTimer != NULL)
    {
        g_Log.WriteError("HTTP base server re-create checkout timer.");
        return true;
    }

    m_CheckoutTimer = event_new(m_EventBase, -1, EV_PERSIST, CallBack_Checkout, this);
    if (m_CheckoutTimer == NULL)
    {
        g_Log.WriteError("HTTP base server create chekcout timer failed.");
        return false;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = TimerInternal;

    if (event_add(m_CheckoutTimer, &tv) != 0)
    {
        g_Log.WriteError("HTTP base server add checkout timer failed.");
        event_free(m_CheckoutTimer);
        m_CheckoutTimer = NULL;
        return false;
    }

    return true;
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

    std::string LogString("Header:\n");
    struct evkeyvalq *Headers = evhttp_request_get_input_headers(Request);
    for (struct evkeyval *header = Headers->tqh_first; header != NULL; header = header->next.tqe_next)
    {
        LogString.append(BoostFormat("\t%s: %s\n", header->key, header->value));
    }

    LogString.erase(LogString.end() - 1);
    g_Log.WriteDebug(LogString);
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
    struct evbuffer *evb = evbuffer_new();
    if (evb == NULL)
    {
        g_Log.WriteError("HTTP base server process directory: evbuffer create failed.");
        return false;
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

    evbuffer_add_printf(evb, "</ul></body></html>\n");
    evhttp_add_header(evhttp_request_get_output_headers(Request), "Content-Type", "text/html");

    evhttp_send_reply(Request, HTTP_OK, "OK", evb);
    g_Log.WriteDebug(LogString);
    evbuffer_free(evb);
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

    struct evbuffer *evb = evbuffer_new();
    if (evb == NULL)
    {
        g_Log.WriteError("HTTP base server process file: evbuffer create failed.");
        close(FileDescriptor);
        return false;
    }

    const char *FileExtension = strrchr(ActualllyPath.c_str(), '.') + 1;
    std::map<std::string, std::string>::iterator it = m_ContentTypeMap.find(FileExtension);
    const char *FileType = (it != m_ContentTypeMap.end()) ? it->second.c_str() : m_ContentTypeMap["misc"].c_str();

    evhttp_add_header(evhttp_request_get_output_headers(Request), "Content-Type", FileType);
    evbuffer_add_file(evb, FileDescriptor, 0, FileStat.st_size); //evbuffer_add_file will close file descriptor after succeed
    evhttp_send_reply(Request, HTTP_OK, "OK", evb);

    evbuffer_free(evb);
    return true;
}

SSL_CTX* HTTPBaseServer::CreateSSLContext(const char *CertFile, const char *KeyFile, const char *CaFile)
{
    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD *meth = SSLv23_server_method();
    SSL_CTX *ctx = SSL_CTX_new(meth);
    if (NULL == ctx)
    {
        g_Log.WriteError("HTTP base server could not new SSL_CTX.");
        return NULL;
    }
    if (SSL_CTX_load_verify_locations(ctx, CaFile, NULL) <= 0)
    {
        g_Log.WriteError("HTTP base server could not load ca cert file.");
    }
    if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
    {
        g_Log.WriteError("HTTP base server could not use certificate file.");
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0)
    {
        g_Log.WriteError("HTTP base server could not use private key file.");
    }
    if (!SSL_CTX_check_private_key(ctx))
    {
        g_Log.WriteError("HTTP base server could private key does not match certfile.");
    }
    return ctx;
}
void HTTPBaseServer::CallBack_GenericRequest(evhttp_request * Request, void * arg)
{
    HTTPBaseServer *Server = (HTTPBaseServer*)arg;
    Server->ProcessRequest(Request);
}

void HTTPBaseServer::CallBack_Checkout(int Socket, short Events, void * UserData)
{
    HTTPBaseServer *Server = (HTTPBaseServer*)UserData;
    Server->ProcessCheckout();

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 5;          //TimerInternal
    event_add(Server->m_CheckoutTimer, &tv);
}
bufferevent * HTTPBaseServer::CallBack_Bufferevent(event_base *base, void *arg)
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
        g_Log.WriteError("HTTP base server new bufferevent failed.");
    }
    return SSLbev;
}
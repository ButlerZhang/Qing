#pragma once
#include <map>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <event.h>
#include <event2/http.h>

std::map<std::string, std::string> g_ContentTypeMap;



void PrintRequest(struct evhttp_request* Request)
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

    printf("Received a %s request for: %s\nHeaders:\n", RequestType, evhttp_request_get_uri(Request));

    struct evkeyvalq *Headers = evhttp_request_get_input_headers(Request);
    for (struct evkeyval *header = Headers->tqh_first; header != NULL; header = header->next.tqe_next)
    {
        printf("\t%s: %s\n", header->key, header->value);
    }

    printf("\r\n");
}

bool ParseRequestPath(struct evhttp_request* Request, std::string &ActualllyPath)
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

    if (strcmp(DecodeURI, "/") == 0)
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

void ProcessDirectory(struct evhttp_request *Request, const std::string &ActualllyPath)
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

void ProcessFile(struct evhttp_request *Request, struct stat &FileStat, const std::string &ActualllyPath)
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
    std::map<std::string, std::string>::iterator it = g_ContentTypeMap.find(FileExtension);
    const char *FileType = (it != g_ContentTypeMap.end()) ? it->second.c_str() : g_ContentTypeMap["misc"].c_str();
    printf("\r\nFile:\r\n\tExtension = %s\n\tType = %s\n\n", FileExtension, FileType);

    evhttp_add_header(evhttp_request_get_output_headers(Request), "Content-Type", FileType);
    evbuffer_add_file(evb, FileDescriptor, 0, FileStat.st_size);
    evhttp_send_reply(Request, HTTP_OK, "OK", evb);

    close(FileDescriptor);
    evbuffer_free(evb);
}

void CallBack5_GenericRequest(struct evhttp_request *Request, void *arg)
{
    PrintRequest(Request);

    std::string FullPath;
    if (!ParseRequestPath(Request, FullPath))
    {
        return;
    }

    struct stat ActuallyPathStat;
    if (stat(FullPath.c_str(), &ActuallyPathStat) < 0)
    {
        printf("ERROR: Stat actually path failed.\n\n");
        return;
    }

    if (S_ISDIR(ActuallyPathStat.st_mode))
    {
        ProcessDirectory(Request, FullPath);
    }
    else
    {
        ProcessFile(Request, ActuallyPathStat, FullPath);
    }
}

void demo5_server_evhttp(const char *ServerIP, int Port)
{
    //initialize content type map
    {
        g_ContentTypeMap["txt"] = "text/plain";
        g_ContentTypeMap["c"] = "text/plain";
        g_ContentTypeMap["h"] = "text/plain";
        g_ContentTypeMap["html"] = "text/html";
        g_ContentTypeMap["htm"] = "text/htm";
        g_ContentTypeMap["css"] = "text/css";
        g_ContentTypeMap["gif"] = "image/gif";
        g_ContentTypeMap["jpg"] = "image/jpeg";
        g_ContentTypeMap["jpeg"] = "image/jpeg";
        g_ContentTypeMap["png"] = "image/png";
        g_ContentTypeMap["ogg"] = "application/ogg";
        g_ContentTypeMap["pdf"] = "application/pdf";
        g_ContentTypeMap["ps"] = "application/postscript";
        g_ContentTypeMap["au"] = "audio/basic";
        g_ContentTypeMap["wav"] = "audio/wav";
        g_ContentTypeMap["mid"] = "audio/midi";
        g_ContentTypeMap["midi"] = "audio/midi";
        g_ContentTypeMap["mp3"] = "audio/mpeg";
        g_ContentTypeMap["avi"] = "video/x-msvideo";
        g_ContentTypeMap["qt"] = "video/quicktime";
        g_ContentTypeMap["mov"] = "video/quicktime";
        g_ContentTypeMap["mpeg"] = "video/mpeg";
        g_ContentTypeMap["mpe"] = "video/mpeg";
        g_ContentTypeMap["vrml"] = "model/vrml";
        g_ContentTypeMap["misc"] = "application/misc";
    }

    struct event_base *base = event_base_new();
    if (base == NULL)
    {
        printf("ERROR: event init failed.\n");
        return;
    }

    struct evhttp * http = evhttp_new(base);
    if (http == NULL)
    {
        printf("ERROR: could not create evhttp.\n");
        event_base_free(base);
        return;
    }

    if (evhttp_bind_socket(http, ServerIP, static_cast<uint16_t>(Port)) != 0)
    {
        printf("ERROR: http server bind failed.\n");
        event_base_free(base);
        return;
    }

    evhttp_set_timeout(http, 5);
    evhttp_set_gencb(http, CallBack5_GenericRequest, NULL);

    printf("HTTP Server start dispatch...\n\n");
    event_base_dispatch(base);
    event_base_free(base);
}

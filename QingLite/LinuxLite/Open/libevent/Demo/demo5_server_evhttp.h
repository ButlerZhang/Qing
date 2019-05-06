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

std::string g_WorkDirectory;
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
        printf("    %s: %s\n", header->key, header->value);
    }
}

bool ParseRequestPath(struct evhttp_request* Request, std::string &FullPath)
{
    const char *URI = evhttp_request_get_uri(Request);
    printf("URI = %s\n", URI);

    struct evhttp_uri *Decoded = evhttp_uri_parse(URI);
    if (Decoded == NULL)
    {
        printf("It is not a good URI. Sending BADREQUEST.\n");
        evhttp_send_error(Request, HTTP_BADREQUEST, "Can not decoded URI.");
        return false;
    }

    const char *RequestPath = evhttp_uri_get_path(Decoded);
    printf("Request path = %s\n", RequestPath);
    if (RequestPath == NULL)
    {
        RequestPath = "/";
    }

    char *DecodedPath = evhttp_uridecode(RequestPath, 0, NULL);
    if (DecodedPath == NULL)
    {
        evhttp_send_error(Request, HTTP_NOTFOUND, "Document was not found");
        printf("Decoded path is NULL.\n");
        evhttp_uri_free(Decoded);
        return false;
    }

    if (strstr(DecodedPath, ".."))
    {
        evhttp_send_error(Request, HTTP_NOTFOUND, "Document was not found");
        printf("Decoded path include '..' directory.\n");
        evhttp_uri_free(Decoded);
        free(DecodedPath);
        return false;
    }

    printf("Request decoded path = %s\n", DecodedPath);
    FullPath = g_WorkDirectory + DecodedPath;
    if (strcmp(DecodedPath, "/") == 0)
    {
        //FullPath += "index.html";
    }

    printf("Whole path = %s\n", FullPath.c_str());
    evhttp_uri_free(Decoded);
    free(DecodedPath);
    return true;
}

void ProcessDirectory(struct evhttp_request *Request, const std::string &FullPath)
{
    printf("Process directory.\n");

    DIR *Directory = opendir(FullPath.c_str());
    if (Directory == NULL)
    {
        printf("ERROR: Open directory failed.\n");
        return;
    }

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
        "  <title>HTTP Server Test</title>\n"
        "  <base href='Directory'>\n"
        " </head>\n"
        " <body>\n"
        "  <h1>list</h1>\n"
        "  <ul>\n");

    struct dirent *ent = NULL;
    while ((ent = readdir(Directory)))
    {
        const char *name = ent->d_name;
        if (strcmp(name, "..") != 0 && strcmp(name, ".") != 0)
        {
            evbuffer_add_printf(evb, "   <li><a href=\"%s\">%s</a>\n", name, name);
        }
    }

    evbuffer_add_printf(evb, "</ul></body></html>\n");
    closedir(Directory);

    evhttp_add_header(evhttp_request_get_output_headers(Request), "Content-Type", "text/html");
    evhttp_send_reply(Request, HTTP_OK, "OK", evb);
    evbuffer_free(evb);
}

void ProcessFile(struct evhttp_request *Request, struct stat &st, const std::string &FullPath)
{
    printf("Process file.\n");

    int FileDescriptor = open(FullPath.c_str(), O_RDONLY);
    if (FileDescriptor <= 0)
    {
        printf("ERROR: Open file descriptor failed.\n");
        return;
    }

    if (fstat(FileDescriptor, &st) < 0)
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

    const char *extension = strrchr(FullPath.c_str(), '.') + 1;
    printf("File extension = %s\n", extension);

    std::map<std::string, std::string>::iterator it = g_ContentTypeMap.find(extension);
    const char *FileType = (it != g_ContentTypeMap.end()) ? it->second.c_str() : g_ContentTypeMap["misc"].c_str();
    printf("File type = %s\n", FileType);

    evhttp_add_header(evhttp_request_get_output_headers(Request), "Content-Type", FileType);
    evbuffer_add_file(evb, FileDescriptor, 0, st.st_size);
    evhttp_send_reply(Request, HTTP_OK, "OK", evb);

    close(FileDescriptor);
    evbuffer_free(evb);
}

void CallBack5_GenericRequest(struct evhttp_request *Request, void *arg)
{
    printf("Process generic request begin...\n");
    PrintRequest(Request);

    std::string FullPath;
    if (!ParseRequestPath(Request, FullPath))
    {
        return;
    }

    struct stat st;
    if (stat(FullPath.c_str(), &st) < 0)
    {
        printf("ERROR: stat full path failed.\n\n");
        return;
    }

    if (S_ISDIR(st.st_mode))
    {
        ProcessDirectory(Request, FullPath);
    }
    else
    {
        ProcessFile(Request, st, FullPath);
    }

    printf("Process generic request end...\n\n");
}

void demo5_server_evhttp(const char *ServerIP, int Port)
{
    //get work direcotry
    {
        char WorkPath[PATH_MAX];
        if (getcwd(WorkPath, PATH_MAX) == NULL)
        {
            printf("ERROR: Get work path failed.\n");
            return;
        }

        g_WorkDirectory = WorkPath;
        printf("Work Path = %s\n", WorkPath);
    }

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
        g_ContentTypeMap["pdf"] = "application/pdf";
        g_ContentTypeMap["ps"] = "application/postscript";
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
        return;
    }

    if (evhttp_bind_socket(http, ServerIP, static_cast<uint16_t>(Port)) != 0)
    {
        printf("ERROR: http server bind failed.\n");
        return;
    }

    evhttp_set_timeout(http, 5);
    evhttp_set_gencb(http, CallBack5_GenericRequest, NULL);

    printf("HTTP Server start dispatch...\n\n");
    event_base_dispatch(base);
}

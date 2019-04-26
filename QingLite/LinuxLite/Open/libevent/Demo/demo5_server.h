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



void CallBack_DocumentRequest(struct evhttp_request* Request, void *arg)
{
    printf("Start process document request\n");

    const char *cmdtype;
    switch (evhttp_request_get_command(Request))
    {
    case EVHTTP_REQ_GET:        cmdtype = "GET";        break;
    case EVHTTP_REQ_POST:       cmdtype = "POST";       break;
    case EVHTTP_REQ_HEAD:       cmdtype = "HEAD";       break;
    case EVHTTP_REQ_PUT:        cmdtype = "PUT";        break;
    case EVHTTP_REQ_DELETE:     cmdtype = "DELETE";     break;
    case EVHTTP_REQ_OPTIONS:    cmdtype = "OPTIONS";    break;
    case EVHTTP_REQ_TRACE:      cmdtype = "TRACE";      break;
    case EVHTTP_REQ_CONNECT:    cmdtype = "CONNECT";    break;
    case EVHTTP_REQ_PATCH:      cmdtype = "PATCH";      break;
    default:                    cmdtype = "unknown";    break;
    }

    printf("Received a %s request for: %s\nHeaders:\n",
        cmdtype, evhttp_request_get_uri(Request));

    struct evkeyvalq *headers = evhttp_request_get_input_headers(Request);
    for (struct evkeyval *header = headers->tqh_first; header != NULL; header = header->next.tqe_next)
    {
        printf("  %s: %s\n", header->key, header->value);
    }

    struct evbuffer *InputBuf = evhttp_request_get_input_buffer(Request);
    puts("\n  Input buf data: ");
    while (evbuffer_get_length(InputBuf))
    {
        int n;
        char cbuf[128];
        n = evbuffer_remove(InputBuf, cbuf, sizeof(cbuf));
        if (n > 0)
            (void) fwrite(cbuf, 1, n, stdout);
    }
    puts("\n");

    struct evbuffer *ReplyBuf = evbuffer_new();
    if (ReplyBuf == NULL)
    {
        printf("evbuffer_new error !\n");
        return;
    }

    evbuffer_add_printf(ReplyBuf, "<html>\n"
        "<head>\n"
        "  <title>HTTP Server</title>\n"
        "</head>\n"
        "<body>\n"
        "  <h1>Let it go.</h1>\n"
        "</body>\n"
        "</html>\n");
    //evbuffer_add_printf(ReplyBuf, "Hello");

    evhttp_send_reply(Request, HTTP_OK, "OK", ReplyBuf);
    evbuffer_free(ReplyBuf);

    printf("Stop process document request\n\n");
}

void CallBack_GenericRequest(struct evhttp_request *Request, void *arg)
{
    printf("Start process generic request\n");
    if (evhttp_request_get_command(Request) != EVHTTP_REQ_GET)
    {
        CallBack_DocumentRequest(Request, arg);
        return;
    }

    const char *URI = evhttp_request_get_uri(Request);
    printf("Got a GET request for : %s\n", URI);

    /* Decode the URI */
    struct evhttp_uri *Decoded = evhttp_uri_parse(URI);
    if (Decoded == NULL)
    {
        printf("It is not a good URI. Sending BADREQUEST\n");
        evhttp_send_error(Request, HTTP_BADREQUEST, "Can not decoded URI.");
        return;
    }

    /* Let's see what path the user asked for. */
    const char *RequestPath = evhttp_uri_get_path(Decoded);
    printf("User request path = %s\n", RequestPath);
    if (RequestPath == NULL)
    {
        RequestPath = "/";
    }

    /* We need to decode it, to see what path the user really wanted. */
    char *decoded_path = evhttp_uridecode(RequestPath, 0, NULL);
    if (decoded_path == NULL || strstr(decoded_path, ".."))
    {
        evhttp_send_error(Request, HTTP_NOTFOUND, "Document was not found");
        return;
    }

    char WorkPath[PATH_MAX];
    if (getcwd(WorkPath, PATH_MAX) == NULL)
    {
        printf("ERROR: Get work path failed.\n");
        return;
    }

    size_t PathLength = strlen(decoded_path) + strlen(WorkPath) + 2;
    printf("Work Path = %s\n", WorkPath);
    printf("User request ture path = %s\n", decoded_path);

    char WholePath[PATH_MAX];
    if (strcmp(decoded_path, (const char*)("/")) == 0)
    {
        evutil_snprintf(WholePath, PathLength, "%s/", WorkPath);
    }
    else
    {
        evutil_snprintf(WholePath, PathLength, "%s/%s", WorkPath, decoded_path);
    }

    printf("Whole path = %s\n", WholePath);

    struct stat st;
    if (stat(WholePath, &st) < 0)
    {
        printf("stat whole_path failed.\n\n");
        return;
    }

    /* This holds the content we're sending. */
    struct evbuffer *evb = evbuffer_new();
    if (evb == NULL)
    {
        printf("ERROR: evbuffer create failed.\n");
        return;
    }

    if (S_ISDIR(st.st_mode))
    {
        printf("Enter directory\n");
        DIR *Directory = opendir(WholePath);
        if (Directory == NULL)
        {
            printf("ERROR: opendir failed.\n");
            return;
        }

        evbuffer_add_printf(evb,
            "<!DOCTYPE html>\n"
            "<html>\n <head>\n"
            "  <meta charset='utf-8'>\n"
            "  <title>%s</title>\n"
            "  <base href='%s'>\n"
            " </head>\n"
            " <body>\n"
            "  <h1>%s</h1>\n"
            "  <ul>\n",
            decoded_path, /* XXX html-escape this. */
            RequestPath, /* XXX html-escape this? */
            decoded_path /* XXX html-escape this */);

        struct dirent *ent = NULL;
        while ((ent = readdir(Directory)))
        {
            const char *name = ent->d_name;
            if (strcmp(name, "..") != 0 && strcmp(name, ".") != 0)
            {
                evbuffer_add_printf(evb, "   <li><a href=\"%s\">%s</a>\n",
                    name, name);/* XXX escape this */
            }
        }

        evbuffer_add_printf(evb, "</ul></body></html>\n");
        closedir(Directory);

        evhttp_add_header(evhttp_request_get_output_headers(Request),
            "Content-Type", "text/html");
    }
    else
    {
        printf("It is File\n");
        int FileDescriptor = open(WholePath, O_RDONLY);
        if (FileDescriptor <= 0)
        {
            printf("ERROR: open failed.\n");
            return;
        }

        if (fstat(FileDescriptor, &st) < 0)
        {
            printf("ERROR: fstat failed.\n");
            return;
        }

        static std::map<std::string, std::string> ContentTypeMap;
        if (ContentTypeMap.empty())
        {
            ContentTypeMap["txt"] = "text/plain";
            ContentTypeMap["c"] = "text/plain";
            ContentTypeMap["h"] = "text/plain";
            ContentTypeMap["html"] = "text/html";
            ContentTypeMap["htm"] = "text/htm";
            ContentTypeMap["css"] = "text/css";
            ContentTypeMap["gif"] = "image/gif";
            ContentTypeMap["jpg"] = "image/jpeg";
            ContentTypeMap["jpeg"] = "image/jpeg";
            ContentTypeMap["png"] = "image/png";
            ContentTypeMap["pdf"] = "application/pdf";
            ContentTypeMap["ps"] = "application/postscript";
            ContentTypeMap["misc"] = "application/misc";

            printf("Content type map size = %d\n", static_cast<int>(ContentTypeMap.size()));
        }

        const char *extension = strrchr(decoded_path, '.') + 1;
        printf("extension = %s\n", extension);

        std::map<std::string, std::string>::iterator it = ContentTypeMap.find(extension);
        const char *type = (it != ContentTypeMap.end()) ? it->second.c_str() : ContentTypeMap["misc"].c_str();
        printf("File type = %s\n", type);

        evhttp_add_header(evhttp_request_get_output_headers(Request), "Content-Type", type);
        evbuffer_add_file(evb, FileDescriptor, 0, st.st_size);
    }

    evhttp_send_reply(Request, HTTP_OK, "OK", evb);
    printf("Stop process generic request\n\n");
}

void demo5_server(const char *ServerIP, int Port)
{
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

    evhttp_set_gencb(http, CallBack_GenericRequest, NULL);
    evhttp_set_cb(http, "/dump", CallBack_DocumentRequest, NULL);

    printf("HTTP Server start dispatch...\n\n");
    event_base_dispatch(base);
}

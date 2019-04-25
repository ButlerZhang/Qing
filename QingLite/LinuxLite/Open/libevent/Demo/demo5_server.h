#pragma once
#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <event2/http_compat.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>


const struct table_entry {
    const char *extension;
    const char *content_type;
} content_type_table[] = {
    { "txt", "text/plain" },
    { "c", "text/plain" },
    { "h", "text/plain" },
    { "html", "text/html" },
    { "htm", "text/htm" },
    { "css", "text/css" },
    { "gif", "image/gif" },
    { "jpg", "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "png", "image/png" },
    { "pdf", "application/pdf" },
    { "ps", "application/postscript" },
    { NULL, NULL },
};

const char* guess_content_type(const char *path)
{
    const char *last_period = strrchr(path, '.');
    if (!last_period || strchr(last_period, '/'))
        return "application/misc";

    const char *extension = last_period + 1;
    for (const table_entry *ent = &content_type_table[0]; ent->extension; ++ent)
    {
        if (!evutil_ascii_strcasecmp(ent->extension, extension))
        {
            return ent->content_type;
        }
    }

    return NULL;
}

void CallBack_DocumentRequest(struct evhttp_request* Request, void *arg)
{
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

    printf("Received a %s request for %s\nHeaders:\n",
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
    puts("\n\n");

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
}

void CallBack_GenericHandler(struct evhttp_request *Request, void *arg)
{
    if (evhttp_request_get_command(Request) != EVHTTP_REQ_GET)
    {
        CallBack_DocumentRequest(Request, arg);
        return;
    }

    const char *URI = evhttp_request_get_uri(Request);
    printf("Got a GET request for <%s>\n", URI);

    /* Decode the URI */
    struct evhttp_uri *Decoded = evhttp_uri_parse(URI);
    if (Decoded == NULL)
    {
        printf("It is not a good URI. Sending BADREQUEST\n");
        evhttp_send_error(Request, HTTP_BADREQUEST, "Can not decoded URI.");
        return;
    }

    /* Let's see what path the user asked for. */
    const char *path = evhttp_uri_get_path(Decoded);
    printf("evhttp_uri_get_path, path = %s\n", path);
    if (path == NULL)
    {
        path = "/";
    }

    /* We need to decode it, to see what path the user really wanted. */
    char *decoded_path = evhttp_uridecode(path, 0, NULL);
    if (decoded_path == NULL || strstr(decoded_path, ".."))
    {
        evhttp_send_error(Request, HTTP_NOTFOUND, "Document was not found");
        return;
    }

    const char *docroot = (const char*)arg;
    size_t len = strlen(decoded_path) + strlen(docroot) + 2;

    printf("evhttp_uridecode, decoded_path = %s\n", decoded_path);
    printf("docroot = %s\n", docroot);

    char whole_path[1024];
    memset(whole_path, 0, sizeof(whole_path));
    //evutil_snprintf(whole_path, len, "%s", decoded_path);
    evutil_snprintf(whole_path, len, "%s/%s", docroot, decoded_path);
    printf("whole_path = %s.\n", whole_path);

    struct stat st;
    if (stat(whole_path, &st) < 0)
    {
        printf("stat whole_path failed.\n");
        return;
    }

    /* This holds the content we're sending. */
    struct evbuffer *evb = evbuffer_new();

    if (S_ISDIR(st.st_mode))
    {
        printf("S_ISDIR\n");
        const char *trailing_slash = "";
        if (!strlen(path) || path[strlen(path) - 1] != '/')
        {
            trailing_slash = "/";
        }

        DIR *d = opendir(whole_path);
        if (d == NULL)
        {
            printf("ERROR: opendir failed.\n");
            return;
        }

        evbuffer_add_printf(evb,
            "<!DOCTYPE html>\n"
            "<html>\n <head>\n"
            "  <meta charset='utf-8'>\n"
            "  <title>%s</title>\n"
            "  <base href='%s%s'>\n"
            " </head>\n"
            " <body>\n"
            "  <h1>%s</h1>\n"
            "  <ul>\n",
            decoded_path, /* XXX html-escape this. */
            path, /* XXX html-escape this? */
            trailing_slash,
            decoded_path /* XXX html-escape this */);

        struct dirent *ent = NULL;
        while ((ent = readdir(d)))
        {
            const char *name = ent->d_name;
            evbuffer_add_printf(evb, "    <li><a href=\"%s\">%s</a>\n",
                name, name);/* XXX escape this */
        }

        evbuffer_add_printf(evb, "</ul></body></html>\n");
        closedir(d);

        evhttp_add_header(evhttp_request_get_output_headers(Request),
            "Content-Type", "text/html");
    }
    else
    {
        printf("It is File\n");
        int fd = open(whole_path, O_RDONLY);
        if (fd <= 0)
        {
            printf("ERROR: open failed.\n");
            return;
        }

        if (fstat(fd, &st) < 0)
        {
            printf("ERROR: fstat failed.\n");
            return;
        }

        const char *type = guess_content_type(decoded_path);
        evhttp_add_header(evhttp_request_get_output_headers(Request), "Content-Type", type);
        evbuffer_add_file(evb, fd, 0, st.st_size);
    }

    evhttp_send_reply(Request, HTTP_OK, "OK", evb);
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

    const char *Dump = "/home/butler/Butler/Code/Deployment/QingLite/LinuxLite/bin/x64/Debug/";
    evhttp_set_gencb(http, CallBack_GenericHandler, (void*)Dump);
    evhttp_set_cb(http, "/dump", CallBack_DocumentRequest, NULL);

    printf("HTTP Server start dispatch...\n\n");
    event_base_dispatch(base);
}

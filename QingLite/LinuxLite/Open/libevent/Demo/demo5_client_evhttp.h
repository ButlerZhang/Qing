#pragma once
#include <string.h>
#include <unistd.h>
#include <event.h>
#include <event2/dns.h>
#include <event2/http.h>
#include <event2/event_struct.h>

struct evdns_base* g_DNSBase;



void Callback5_RemoteRead(struct evhttp_request* Request, void* arg)
{
    printf("Remote read done.\n\n");
    //event_base_loopexit((struct event_base*)arg, NULL);
}

int Callback5_ReadHeaderDone(struct evhttp_request* Request, void* arg)
{
    int ResponseCode = evhttp_request_get_response_code(Request);
    const char *ResponseDescirption = evhttp_request_get_response_code_line(Request);
    printf("< HTTP/1.1 %d %s\n", ResponseCode, ResponseDescirption);

    struct evkeyvalq* Headers = evhttp_request_get_input_headers(Request);
    for (struct evkeyval *header = Headers->tqh_first; header != NULL; header = header->next.tqe_next)
    {
        printf("< %s: %s\n", header->key, header->value);
    }

    printf("\n");
    return 0;
}

void Callback5_ReadChunk(struct evhttp_request* Request, void* arg)
{
    const int ReadMaxSisze = 4096;
    char ReadBuffer[ReadMaxSisze];
    memset(ReadBuffer, 0, sizeof(ReadBuffer));

    int ReadByte = 0;
    struct evbuffer* evbuf = evhttp_request_get_input_buffer(Request);
    while ((ReadByte = evbuffer_remove(evbuf, ReadBuffer, ReadMaxSisze)) > 0)
    {
        fwrite(ReadBuffer, ReadByte, 1, stdout);
    }

    printf("\n");
}

void Callback5_RemoteRequestError(enum evhttp_request_error error, void* arg)
{
    printf("ERROR: Remote request error.\n\n");
    //event_base_loopexit((struct event_base*)arg, NULL);
}

void Callback5_RemoteConnectionClose(struct evhttp_connection* connection, void* arg)
{
    printf("Remote connection closed.\n\n");
    //event_base_loopexit((struct event_base*)arg, NULL);
}

void CallBack5_InputFromCMD(int Input, short events, void *arg)
{
    char Message[1024];
    read(Input, Message, sizeof(Message));
    std::string InputChoice(Message);

    const char* OriginalURL = NULL;
    if (InputChoice == "1")          //get
    {
        OriginalURL = "http://192.168.3.126:12345/dump/";
    }
    else if (InputChoice == "2")     //post
    {

    }
    else                             //index
    {
        OriginalURL = "http://192.168.3.126:12345";
    }

    if (OriginalURL == NULL)
    {
        printf("ERROR: original url is empty.\n");
        return;
    }

    struct evhttp_uri* OriginalURI = evhttp_uri_parse(OriginalURL);
    if (OriginalURI == NULL)
    {
        printf("ERROR: parse url failed.\n");
        return;
    }

    const char* Host = evhttp_uri_get_host(OriginalURI);
    if (Host == NULL)
    {
        printf("ERROR: parse host failed.\n");
        evhttp_uri_free(OriginalURI);
        return;
    }

    const char* RequestURL = OriginalURL;
    const char* RequestPath = evhttp_uri_get_path(OriginalURI);
    if (RequestPath == NULL || strlen(RequestPath) == 0)
    {
        RequestURL = (char*)"/";
        printf("come here???\n");
    }

    struct event_base* base = (event_base*)arg;
    struct evhttp_request* Request = evhttp_request_new(Callback5_RemoteRead, base);
    if (Request == NULL)
    {
        printf("ERROR: create http request failed.\n");
        return;
    }

    evhttp_request_set_chunked_cb(Request, Callback5_ReadChunk);
    evhttp_request_set_header_cb(Request, Callback5_ReadHeaderDone);
    evhttp_request_set_error_cb(Request, Callback5_RemoteRequestError);

    int PortFromURI = evhttp_uri_get_port(OriginalURI);
    struct evhttp_connection* g_Connection = evhttp_connection_base_new(base, g_DNSBase, Host, static_cast<uint16_t>(PortFromURI));
    if (g_Connection == NULL)
    {
        printf("ERROR: create http connection failed.\n");
        evhttp_uri_free(OriginalURI);
        evhttp_request_free(Request);
        event_base_free(base);
        return;
    }

    evhttp_connection_set_closecb(g_Connection, Callback5_RemoteConnectionClose, base);
    evhttp_add_header(evhttp_request_get_output_headers(Request), "Host", Host);
    evhttp_make_request(g_Connection, Request, EVHTTP_REQ_GET, RequestURL);

    printf("url:%s\nhost:%s\nport:%d\npath:%s\nRequest url:%s\n\n\n", OriginalURL, Host, PortFromURI, RequestPath, RequestURL);
}

void demo5_client_evhttp(const char *ServerIP, int Port)
{
    struct event_base* base = event_base_new();
    if (base == NULL)
    {
        printf("ERROR: event init failed.\n");
        return;
    }

    g_DNSBase = evdns_base_new(base, 1);
    if (g_DNSBase == NULL)
    {
        printf("ERROR: create DNS base failed.\n");
        event_base_free(base);
        return;
    }

    struct event *ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack5_InputFromCMD, base);
    event_add(ev_cmd, NULL);

    printf("Client start dispatch...\n\n");
    event_base_dispatch(base);

    printf("Client stop dispatch, release resources.\n");
    event_base_free(base);
}

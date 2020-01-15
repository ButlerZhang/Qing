#pragma once
#include <string>
#include <string.h>
#include <unistd.h>
#include <event.h>
#include <event2/dns.h>
#include <event2/http.h>
#include <event2/event_struct.h>



struct evhttp_connection* g_Connection;

void Callback5_RemoteRead(struct evhttp_request* Request, void* arg)
{
    printf("Remote read done.\n");
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

void Callback7_ReadChunk(struct evhttp_request* Request, void* arg)
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
    char Buffer[1024];
    ssize_t ReadSize = read(Input, Buffer, sizeof(Buffer));

    Buffer[ReadSize] = '\0';
    int Choice = atoi(Buffer);
    printf("==========User choice = %d==========\n", Choice);

    struct event_base* base = (event_base*)arg;
    struct evhttp_request* Request = evhttp_request_new(Callback5_RemoteRead, base);
    if (Request == NULL)
    {
        printf("ERROR: Create request failed.\n");
        return;
    }

    evhttp_request_set_chunked_cb(Request, Callback7_ReadChunk);
    evhttp_request_set_header_cb(Request, Callback5_ReadHeaderDone);
    evhttp_request_set_error_cb(Request, Callback5_RemoteRequestError);
    evhttp_add_header(evhttp_request_get_output_headers(Request), "Host", "192.168.3.126");

    switch (Choice)
    {
        case 1://get directory or file
        {
            sprintf(Buffer, "http://192.168.3.126:12345");
            evhttp_make_request(g_Connection, Request, EVHTTP_REQ_GET, Buffer);
            break;
        }

        case 2://post
        {
            const std::string PostData("You are stupid.");
            evbuffer_add(evhttp_request_get_output_buffer(Request), PostData.c_str(), PostData.length());

            sprintf(Buffer, "http://192.168.3.126:12345?user=butler&password=1234567890");
            evhttp_make_request(g_Connection, Request, EVHTTP_REQ_POST, Buffer);
            break;
        }

        case 3://delete
        {
            sprintf(Buffer, "http://192.168.3.126:12345");
            evhttp_make_request(g_Connection, Request, EVHTTP_REQ_DELETE, Buffer);
            break;
        }

        default://get 404.html
        {
            sprintf(Buffer, "http://192.168.3.126:12345/404.html");
            evhttp_make_request(g_Connection, Request, EVHTTP_REQ_GET, Buffer);
            break;
        }
    }
}

void demo5_client_evhttp(const char *ServerIP, int Port)
{
    struct event_base* base = event_base_new();
    if (base == NULL)
    {
        printf("ERROR: event init failed.\n");
        return;
    }

    struct evdns_base* DNSBase = evdns_base_new(base, 1);
    if (DNSBase == NULL)
    {
        printf("ERROR: create DNS base failed.\n");
        event_base_free(base);
        return;
    }

    g_Connection = evhttp_connection_base_new(base, DNSBase, ServerIP, static_cast<uint16_t>(Port));
    if (g_Connection == NULL)
    {
        printf("ERROR: create http connection failed.\n");
        evdns_base_free(DNSBase, 0);
        event_base_free(base);
        return;
    }

    evhttp_connection_set_timeout(g_Connection, 600);
    evhttp_connection_set_retries(g_Connection, -1);
    evhttp_connection_set_closecb(g_Connection, Callback5_RemoteConnectionClose, base);

    struct event *ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, CallBack5_InputFromCMD, base);
    event_add(ev_cmd, NULL);

    printf("Client start dispatch...\n\n");
    event_base_dispatch(base);

    printf("Client stop dispatch, release resources.\n");
    event_base_free(base);
}

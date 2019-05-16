#pragma once
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>

#include <openssl/ssl.h>
#include <event2/event.h>
#include <event2/util.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>



void CallBack6_ServerSSLRead(struct bufferevent *bev, void *arg)
{
    char recvbuf[1024] = { '\0' };
    if (bufferevent_read(bev, recvbuf, 1024) > 0)
    {
        printf("Recv from client: %s\n", recvbuf);
    }
}

void CallBack6_ServerSSLEvent(struct bufferevent *bev, short events, void *arg)
{
    if (events & BEV_EVENT_CONNECTED)
    {
        std::string MSG("Hello");
        printf("Client connected!\n");
        bufferevent_write(bev, MSG.c_str(), MSG.length());
    }
    else if (events & BEV_EVENT_EOF)
    {
        printf("Client disconnected!\n");
        bufferevent_free(bev);
    }
    else
    {
        int err = EVUTIL_SOCKET_ERROR();
        printf("Got an error %d on ssl socket: %s\n", err, evutil_socket_error_to_string(err));
        bufferevent_free(bev);
    }
}

void CallBack6_Accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx)
{
    SSL* ssl = SSL_new((SSL_CTX *)ctx);
    if (ssl == NULL)
    {
        printf("Could not create new SSL\n");
        return;
    }

    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_openssl_socket_new(
        base,
        fd,
        ssl,
        BUFFEREVENT_SSL_ACCEPTING,
        BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL)
    {
        printf("Could not create new ssl bufferevent\n");
        return;
    }

    bufferevent_setcb(bev, CallBack6_ServerSSLRead, NULL, CallBack6_ServerSSLEvent, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

void CallBack6_AcceptError(struct evconnlistener *listener, void *ctx)
{
    int err = EVUTIL_SOCKET_ERROR();
    printf("Got an error %d on listener: %s\n", err, evutil_socket_error_to_string(err));
}

SSL_CTX* CreateSSLContext_Server6(const char *certfile, const char *keyfile, const char *cafile)
{
    /* Load encryption & hashing algorithms for the SSL program */
    SSL_library_init();

    /* Load the error strings for SSL & CRYPTO APIs */
    SSL_load_error_strings();

    /* Create an SSL_METHOD structure (choose an SSL/TLS protocol version) */
    const SSL_METHOD *meth = SSLv23_server_method();

    /* Create an SSL_CTX structure */
    SSL_CTX *ctx = SSL_CTX_new(meth);
    if (NULL == ctx)
    {
        printf("Could not new SSL_CTX\n");
        return NULL;
    }

    /* Load the CA cert file*/
    if (SSL_CTX_load_verify_locations(ctx, cafile, NULL) <= 0)
    {
        printf("Could not load ca cert file\n");
    }

    /* Load the server certificate into the SSL_CTX structure */
    if (SSL_CTX_use_certificate_file(ctx, certfile, SSL_FILETYPE_PEM) <= 0)
    {
        printf("Could not use certificate file\n");
    }

    /* Load the private-key corresponding to the client certificate */
    if (SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM) <= 0)
    {
        printf("Could not use private key file\n");
    }

    /* Check if the client certificate and private-key matches */
    if (!SSL_CTX_check_private_key(ctx))
    {
        printf("Private key does not match certfile\n");
    }

    return ctx;
}

void demo6_server_bufferevent_openssl(const char *ServerIP, int Port)
{
    SSL_CTX *SSLContext = CreateSSLContext_Server6("./server/server-cert.pem","./server/server-key.pem", "./ca/ca-cert.pem");
    if (SSLContext == NULL)
    {
        printf("Create SSL context failed.\n");
        return;
    }

    struct event_base *base = event_base_new();
    if (base == NULL)
    {
        printf("ERROR: event init failed.\n");
        return;
    }

    struct sockaddr_in BindAddress;
    struct sockaddr *sa = (struct sockaddr *)&BindAddress;
    int socklen = sizeof(BindAddress);
    if (evutil_parse_sockaddr_port("192.168.3.126:12345", sa, &socklen) == -1)
    {
        printf("Could not parse address\n");
        return;
    }

    struct evconnlistener *listener = evconnlistener_new_bind(
        base,
        CallBack6_Accept,
        SSLContext,
        LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
        -1,
        sa,
        sizeof(BindAddress));
    if (NULL == listener)
    {
        printf("Could not create listener\n");
        return;
    }

    evconnlistener_set_error_cb(listener, CallBack6_AcceptError);

    printf("Openssl Server start dispatch...\n\n");
    event_base_dispatch(base);
}

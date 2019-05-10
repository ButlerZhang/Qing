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
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>



SSL_CTX* InitClientSSLContext(const char *certfile, const char *keyfile, const char *cafile)
{
    /* Load encryption & hashing algorithms for the SSL program */
    SSL_library_init();

    /* Load the error strings for SSL & CRYPTO APIs */
    SSL_load_error_strings();

    /* Create an SSL_METHOD structure (choose an SSL/TLS protocol version) */
    const SSL_METHOD *meth = SSLv23_client_method();

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

    /* Load the client certificate into the SSL_CTX structure */
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

void CallBack6_SSLRead(struct bufferevent *bev, void *arg)
{
    char recvbuf[1024] = { '\0' };
    if (bufferevent_read(bev, recvbuf, 1024) > 0)
    {
        printf("Recv from client: %s\n", recvbuf);
    }
}

void CallBack6_SSLEvent(struct bufferevent *bev, short events, void *arg)
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

void demo6_client_https(const char *ServerIP, int Port)
{
    SSL_CTX *SSLContext = InitClientSSLContext("./client/client.crt", "./client/client.key", "./ca/ca.cert");
    if (SSLContext == NULL)
    {
        printf("Create SSL context failed.\n");
        return;
    }

    SSL* ssl = SSL_new(SSLContext);
    if (ssl == NULL)
    {
        printf("Could not create new SSL\n");
        return;
    }

    struct event_base *base = event_base_new();
    if (base == NULL)
    {
        printf("ERROR: event init failed.\n");
        return;
    }

    struct bufferevent *bev = bufferevent_openssl_socket_new(
        base,
        -1,
        ssl,
        BUFFEREVENT_SSL_CONNECTING,
        BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL)
    {
        printf("Could not create new ssl bufferevent\n");
        return;
    }

    bufferevent_setcb(bev, CallBack6_SSLRead, NULL, CallBack6_SSLEvent, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    struct sockaddr_in ServerAddress;
    struct sockaddr *sa = (struct sockaddr *)&ServerAddress;
    int socklen = sizeof(ServerAddress);
    if (evutil_parse_sockaddr_port("192.168.3.126:12345", sa, &socklen) == -1)
    {
        printf("Could not parse address\n");
        return;
    }

    if (bufferevent_socket_connect(bev, sa, sizeof(ServerAddress)) == -1)
    {
        int err = EVUTIL_SOCKET_ERROR();
        printf("Could not connect to server: %s\n", evutil_socket_error_to_string(err));
        return;
    }

    printf("HTTPs Client start dispatch...\n\n");
    event_base_dispatch(base);
}

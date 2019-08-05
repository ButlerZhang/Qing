#pragma once
#include <map>
#include <sys/stat.h>
#include <event.h>
#include <event2/http.h>
#include <openssl/ssl.h>



class HTTPBaseServer
{
public:

    HTTPBaseServer();
    virtual ~HTTPBaseServer();
    virtual bool Start(const std::string &ServerIP, int Port, bool IsEnableHTTPS);

    virtual bool ProcessCheckout() { return false; }
    virtual bool ProcessGet(struct evhttp_request *Request);
    virtual bool ProcessPost(struct evhttp_request *Request);
    virtual bool ProcessRequest(struct evhttp_request *Request);

protected:

    bool AddCheckoutTimer(int TimerInternal);
    void PrintRequest(struct evhttp_request* Request);
    bool ParseRequestPath(struct evhttp_request* Request, std::string &ActualllyPath);
    bool ProcessDirectory(struct evhttp_request *Request, const std::string &ActualllyPath);
    bool ProcessFile(struct evhttp_request *Request, struct stat &FileStat, const std::string &ActualllyPath);

    SSL_CTX* CreateSSLContext(const char *CertFile, const char *KeyFile, const char *CaFile);
    static void CallBack_GenericRequest(struct evhttp_request *Request, void *arg);
    static void CallBack_Checkout(int Socket, short Events, void *UserData);

    static bufferevent* CallBack_Bufferevent(struct event_base *base, void *arg);
private:

    struct event_base                                   *m_EventBase;
    struct evhttp                                       *m_evHTTP;
    struct event                                        *m_CheckoutTimer;
    SSL_CTX                                             *m_SSLContext;
    std::map<std::string, std::string>                   m_ContentTypeMap;
};

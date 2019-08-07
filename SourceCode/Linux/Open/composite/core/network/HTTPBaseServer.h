#pragma once
#include "HTTPSession.h"
#include "NetworkBase.h"
#include <map>
#include <sys/stat.h>



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

    HTTPSession& GetHTTPSession() { return m_HTTPSession; }

protected:

    bool AddCheckoutTimer(int TimerInternal);
    void PrintRequest(struct evhttp_request* Request);
    bool GetRequestIPandPort(struct evhttp_connection *Connection, std::string &RequestIP, int &Port);
    bool ParseRequestPath(struct evhttp_request* Request, std::string &ActualllyPath);
    bool ProcessDirectory(struct evhttp_request *Request, const std::string &ActualllyPath);
    bool ProcessFile(struct evhttp_request *Request, struct stat &FileStat, const std::string &ActualllyPath);

    static void CallBack_ConnectionClose(struct evhttp_connection *Connection, void *arg);
    static void CallBack_GenericRequest(struct evhttp_request *Request, void *arg);
    static void CallBack_Checkout(int Socket, short Events, void *UserData);

    static bufferevent* CallBack_Bufferevent(struct event_base *base, void *arg);
private:

    EventBase                                            m_EventBase;
    EventHTTP                                            m_EventHTTP;
    EventNormal                                          m_CheckoutTimer;
    HTTPSession                                          m_HTTPSession;
    std::map<std::string, std::string>                   m_ContentTypeMap;
};

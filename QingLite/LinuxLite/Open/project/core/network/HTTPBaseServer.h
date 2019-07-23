#pragma once
#include <map>
#include <sys/stat.h>
#include <event.h>
#include <event2/http.h>



class HTTPBaseServer
{
public:

    HTTPBaseServer();
    virtual ~HTTPBaseServer();

    virtual bool Start(const std::string &ServerIP, int Port);

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

    static void CallBack_GenericRequest(struct evhttp_request *Request, void *arg);
    static void CallBack_Checkout(int Socket, short Events, void *UserData);

private:

    struct event_base                                   *m_EventBase;
    struct evhttp                                       *m_evHTTP;
    event                                               *m_CheckoutTimer;
    std::map<std::string, std::string>                   m_ContentTypeMap;
};

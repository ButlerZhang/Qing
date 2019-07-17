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

    bool BindBaseEvent(event_base *EventBase);
    virtual bool Start(const std::string &ServerIP, int Port, const std::string &WorkDirectory);

    virtual bool ProcessRequest(struct evhttp_request *Request);
    virtual bool ProcessGet(struct evhttp_request *Request);
    virtual bool ProcessPost(struct evhttp_request *Request);

protected:

    void PrintRequest(struct evhttp_request* Request);
    bool ParseRequestPath(struct evhttp_request* Request, std::string &ActualllyPath);
    bool ProcessDirectory(struct evhttp_request *Request, const std::string &ActualllyPath);
    bool ProcessFile(struct evhttp_request *Request, struct stat &FileStat, const std::string &ActualllyPath);

    static void CallBack_GenericRequest(struct evhttp_request *Request, void *arg);

private:

    std::string                                          m_WorkDirectory;
    struct event_base                                   *m_EventBase;
    struct evhttp                                       *m_evHTTP;
    std::map<std::string, std::string>                   m_ContentTypeMap;
};

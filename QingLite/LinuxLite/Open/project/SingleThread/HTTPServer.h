#pragma once
#include <map>
#include <sys/stat.h>
#include <event.h>
#include <event2/http.h>



class HTTPServer
{
public:

    HTTPServer();
    ~HTTPServer();

    bool BindEventBase(event_base *EventBase);
    bool Start(const std::string &ServerIP, int Port);

private:

    void PrintRequest(struct evhttp_request* Request);
    bool ParseRequestPath(struct evhttp_request* Request, std::string &ActualllyPath);
    void ProcessDirectory(struct evhttp_request *Request, const std::string &ActualllyPath);
    void ProcessFile(struct evhttp_request *Request, struct stat &FileStat, const std::string &ActualllyPath);

    static void CallBack_GenericRequest(struct evhttp_request *Request, void *arg);

private:

    struct event_base                           *m_EventBase;
    struct evhttp                               *m_evHTTP;
    std::map<std::string, std::string>           m_ContentTypeMap;
};

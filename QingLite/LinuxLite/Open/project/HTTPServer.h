#pragma once
#include <vector>
#include "Network/HTTPBaseServer.h"



class HTTPServer : public HTTPBaseServer
{
public:

    HTTPServer();
    ~HTTPServer();

    virtual bool ProcessGet(struct evhttp_request *Request);
    virtual bool ProcessPost(struct evhttp_request *Request);

private:

    bool ProcessUserLogin(struct evhttp_request *Request);
    bool ProcessUserLogout(struct evhttp_request *Request);

    bool GetRequestPath(struct evhttp_request *Request, std::string &RequestPath);
    bool SplitRequestPath(const std::string &RequestPath, std::vector<std::string> &PathVector);
};

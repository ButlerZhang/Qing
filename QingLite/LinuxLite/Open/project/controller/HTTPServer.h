#pragma once
#include <vector>
#include "../handler/User.h"
#include "../core/network/HTTPBaseServer.h"



class HTTPServer : public HTTPBaseServer
{
public:

    HTTPServer();
    ~HTTPServer();

    virtual bool ProcessGet(struct evhttp_request *Request);
    virtual bool ProcessPost(struct evhttp_request *Request);

private:

    bool GetRequestPath(struct evhttp_request *Request, std::string &RequestPath);
    bool SplitRequestPath(const std::string &RequestPath, std::vector<std::string> &PathVector);

private:

    User            m_User;
};

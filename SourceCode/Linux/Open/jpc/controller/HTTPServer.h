#pragma once
#include <vector>
#include "../../../../Common/Database/MySQLDatabase.h"
#include "../core/network/HTTPBaseServer.h"
#include "../handler/httphandler/UserHandler.h"



class HTTPServer : public HTTPBaseServer
{
public:

    virtual bool Start(const std::string &ServerIP, int Port);

    virtual bool ProcessCheckout();
    virtual bool ProcessGet(struct evhttp_request *Request);
    virtual bool ProcessPost(struct evhttp_request *Request);

    MySQLDatabase& GetDB() { return m_HTTPDB; }
    static HTTPServer& GetInstance() { static HTTPServer g_Instance; return g_Instance; }

private:

    HTTPServer();
    virtual ~HTTPServer();

    bool GetRequestPath(struct evhttp_request *Request, std::string &RequestPath);
    bool SplitRequestPath(const std::string &RequestPath, std::vector<std::string> &PathVector);

    static void WorkThread_Process(void *Object);

private:

    bool                                 m_IsWork;
    int                                  m_HTTPPort;
    std::string                          m_ServerIP;
    MySQLDatabase                        m_HTTPDB;
    UserHandler                          m_UserHandler;
};

#define g_HTTPServer HTTPServer::GetInstance()
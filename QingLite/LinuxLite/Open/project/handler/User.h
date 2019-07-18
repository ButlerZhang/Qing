#pragma once

struct evhttp_request;



class User
{
public:

    User();
    ~User();

    bool ProcessLogin(struct evhttp_request *Request);
    bool ProcessLogout(struct evhttp_request *Request);
};
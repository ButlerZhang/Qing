#pragma once
#include "HTTPHandler.h"
#include "../../model/Model.h"

struct evhttp_request;



class UserHandler : public HTTPHandler
{
public:

    UserHandler();
    virtual ~UserHandler();

    bool ProcessLogin(struct evhttp_request *Request);
    bool ProcessLogout(struct evhttp_request *Request);

private:

    bool SendLoginReply(struct evhttp_request *Request, boost::property_tree::ptree &JsonTree, const UserModel &ReplyModel);
};

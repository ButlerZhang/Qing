#pragma once
#include "../Handler.h"
#include "../../model/Model.h"

struct evhttp_request;



class UserHandler : public Handler
{
public:

    UserHandler();
    virtual ~UserHandler();

    bool ProcessLogin(struct evhttp_request *Request);
    bool ProcessLogout(struct evhttp_request *Request);

private:

    bool SendLoginReply(struct evhttp_request *Request, const UserModel &ReplyModel);
};

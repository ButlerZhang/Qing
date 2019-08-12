#pragma once
#include "../../model/Model.h"

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

struct evhttp_request;



class HTTPHandler
{
public:

    HTTPHandler();
    virtual ~HTTPHandler();

    bool ProcessLogin(struct evhttp_request *Request);
    bool ProcessLogout(struct evhttp_request *Request);

protected:

    std::string GetPostDataString(struct evhttp_request * Request);

    std::string GetReplyJsonString(boost::property_tree::ptree &JsonTree);

    bool ParsePostData(boost::property_tree::ptree &JsonTree, const std::string &PostDataString);

    bool SendLoginReply(struct evhttp_request *Request, boost::property_tree::ptree &JsonTree, const UserModel &ReplyModel);
};
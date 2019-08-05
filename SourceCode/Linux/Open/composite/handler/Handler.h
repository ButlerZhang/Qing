#pragma once
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

struct evhttp_request;



class Handler
{
public:

    Handler();
    virtual ~Handler();

protected:

    std::string GetPostDataString(struct evhttp_request * Request);

    std::string GetReplyJsonString(boost::property_tree::ptree &JsonTree);

    bool ParsePostData(boost::property_tree::ptree &JsonTree, const std::string &PostDataString);
};
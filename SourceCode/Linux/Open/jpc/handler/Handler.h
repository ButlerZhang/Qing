#pragma once
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

struct evbuffer;
struct evhttp_request;



class Handler
{
public:

    Handler();
    virtual ~Handler();

protected:

    std::string GetReplyJsonString();
    std::string GetPostDataString(struct evhttp_request * Request);

    bool ParsePostData(const std::string &PostDataString);

protected:

    struct evbuffer                *m_evbuffer;
    boost::property_tree::ptree     m_JsonTree;
};
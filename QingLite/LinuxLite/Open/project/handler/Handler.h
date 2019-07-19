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

    std::string GetPostData(struct evhttp_request * Request);
    bool ParsePostData(const std::string &PostDataString);

protected:

    boost::property_tree::ptree m_JsonTree;
};
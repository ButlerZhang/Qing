#pragma once
#include <string>



class Config
{
public:

    Config();
    ~Config();

public:

    int                     m_DBPort;
    std::string             m_DBHost;
    std::string             m_DBUser;
    std::string             m_DBName;
    std::string             m_DBPassword;
};

extern Config g_Config;
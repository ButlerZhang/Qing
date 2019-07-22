#pragma once
#include <string>



class Config
{
public:

    Config();
    ~Config();

    bool LoadConfig(const std::string &FileName);

private:

    bool LoadFileConfig(const std::string &FileName);
    bool LoadDatabaseConfig();

    bool ParseServerSection(const std::string &ConfigName, const std::string &ConfigValue);

public:

    //Database
    int                     m_DBPort;
    std::string             m_DBHost;
    std::string             m_DBUser;
    std::string             m_DBName;
    std::string             m_DBPassword;

    //server
    int                     m_SMIBPort;
    int                     m_HTTPPort;
    std::string             m_ServerIP;
};

extern Config g_Config;
#pragma once
#include <string>



class Config
{
public:

    Config();
    ~Config();

    void EnterToolMode();
    bool LoadConfig();

private:

    bool LoadFileConfig();
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

private:

    std::string             m_ConfigFileName;
    const std::string       DB_PASSWORD_KEY;
};

extern Config g_Config;
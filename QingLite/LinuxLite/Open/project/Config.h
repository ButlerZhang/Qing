#pragma once
#include <string>



class Config
{
public:

    static Config& GetInstance();

    void GenerateConfigFile();
    bool LoadConfig();

private:

    Config();
    ~Config();

    bool LoadFileConfig();
    bool LoadDatabaseConfig();

    bool ParseDebugSection(const std::string &ConfigName, const std::string &ConfigValue);
    bool ParseServerSection(const std::string &ConfigName, const std::string &ConfigValue);

public:

    //Database
    int                             m_DBPort;
    std::string                     m_DBHost;
    std::string                     m_DBUser;
    std::string                     m_DBName;
    std::string                     m_DBPassword;

    //server
    int                             m_SMIBPort;
    int                             m_HTTPPort;
    std::string                     m_ServerIP;

    //debug
    int                             m_LogSeverity;

private:

    std::string                     m_ConfigFileName;
    const std::string               DB_PASSWORD_KEY;
};

#define g_Config Config::GetInstance()

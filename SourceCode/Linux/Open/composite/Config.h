#pragma once
#include <string>



class Config
{
public:

    void GenerateConfigFile();

    bool LoadConfig();
    bool IsLoadSucceed() const { return m_IsLoadSucceed; }

    static Config& GetInstance() { static Config g_Instance; return g_Instance; }

private:

    Config();
    ~Config();

    bool LoadFileConfig();
    bool LoadDatabaseConfig();
    bool LoadDeviceJPCTable();
    bool LoadServerConfigTable();

    bool ParseConfiguration(const std::string &ConfigName, const std::string &ConfigValue);

public:

    //Database
    int                             m_DBPort;
    std::string                     m_DBHost;
    std::string                     m_DBUser;
    std::string                     m_DBName;
    std::string                     m_DBPassword;

    //server
    bool                            m_IsEnableHTTPS;
    bool                            m_IsEnableLog;
    int                             m_LogSeverity;
    int                             m_SMIBPort;
    int                             m_HTTPPort;
    std::string                     m_ServerIP;

private:

    bool                            m_IsLoadSucceed;
    std::string                     m_ConfigFileName;
};

#define g_Config Config::GetInstance()

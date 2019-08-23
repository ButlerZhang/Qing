#pragma once
#include <string>



class Config
{
public:

    void GenerateConfigFile();

    bool LoadFileConfig();
    bool LoadDatabaseConfig();

    bool IsLoadDBSucceed() const { return m_IsLoadDBSucceed; }
    static Config& GetInstance() { static Config g_Instance; return g_Instance; }

private:

    Config();
    ~Config();

public:

    //Database
    int                             m_DBPort;
    std::string                     m_DBHost;
    std::string                     m_DBUser;
    std::string                     m_DBName;
    std::string                     m_DBPassword;

    //system
    bool                            m_IsEnableHTTPS;
    bool                            m_IsEnableLog;
    bool                            m_IsEnableDaemon;
    int                             m_LogSeverity;
    int                             m_SMIBPort;
    int                             m_HTTPPort;
    std::string                     m_ServerIP;

private:

    bool                            m_IsLoadDBSucceed;
    std::string                     m_ConfigFileName;
};

#define g_Config Config::GetInstance()

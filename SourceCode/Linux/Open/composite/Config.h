#pragma once
#include <string>



class Config
{
public:

    void GenerateConfigFile();

    bool LoadFileConfig();
    bool LoadDatabaseConfig();

    bool IsLoadDBSucceed() const { return m_IsLoadDBSucceed; }
    const std::string& GetProcessName() const { return m_ProcessName; }
    void SetProcessName(const std::string &ProcessName) { m_ProcessName = ProcessName; }

    static Config& GetInstance() { static Config g_Instance; return g_Instance; }

private:

    Config();
    ~Config();
    bool CheckConfiguration();

public:

    //Database
    int                             m_DBPort;
    std::string                     m_DBHost;
    std::string                     m_DBUser;
    std::string                     m_DBName;
    std::string                     m_DBPassword;

    //system
    bool                            m_IsEnableLog;
    bool                            m_IsEnableHTTPS;
    bool                            m_IsEnableDaemon;
    int                             m_LogSeverity;
    int                             m_SMIBPort;
    int                             m_HTTPPort;
    std::string                     m_ServerIP;

    //watchdog
    bool                            m_IsEnableWatchDog;
    bool                            m_IsEnableDeleteLog;
    int                             m_HeartbeatInternal;

public:

    const std::string               JPC_WATCHDOG;
    const std::string               JPC_SERVER;
    const std::string               JPC_SMIB;

private:

    bool                            m_IsLoadDBSucceed;
    std::string                     m_ProcessName;
    std::string                     m_ConfigFileName;
};

#define g_Config Config::GetInstance()

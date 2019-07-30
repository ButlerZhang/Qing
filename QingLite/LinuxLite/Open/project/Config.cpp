#include "Config.h"
#include "../../LinuxTools.h"
#include "core/tools/BoostLog.h"
#include "core/tools/OpenSSLAES.h"
#include "core/database/MySQLDatabase.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>
#include <event.h>



void CallBack_LibEventLog(int Severity, const char *LogMsg)
{
    switch (Severity)
    {
    case EVENT_LOG_DEBUG:           g_Log.WriteDebug(LogMsg);   break;
    case EVENT_LOG_MSG:             g_Log.WriteInfo(LogMsg);    break;
    case EVENT_LOG_WARN:            g_Log.WriteError(LogMsg);   break;
    case EVENT_LOG_ERR:             g_Log.WriteError(LogMsg);   break;
    default:                        g_Log.WriteError(LogMsg);   break;
    }
}



Config::Config() : m_ConfigFileName("project.ini"), DB_PASSWORD_KEY("CJSZHCHCSZCJSZCJ")
{
    //evthread_use_pthreads();
    event_set_log_callback(CallBack_LibEventLog);
}

Config::~Config()
{
}

Config & Config::GetInstance()
{
    static Config g_ConfigInstance;
    return g_ConfigInstance;
}

void Config::GenerateConfigFile()
{
    std::string InputString;
    boost::property_tree::ptree DBTree;
    g_Log.WriteInfo("Enter generate config file mode.");

    std::cout << std::endl << "Input DB Host IP:" << std::endl;
    std::cin >> InputString;
    DBTree.put("Host", InputString);

    std::cout << "Input DB Port:" << std::endl;
    std::cin >> InputString;
    DBTree.put("Port", InputString);

    std::cout << "Input DB User:" << std::endl;
    std::cin >> InputString;
    DBTree.put("User", InputString);

    std::cout << "Input DB Password:" << std::endl;
    std::cin >> InputString;
    DBTree.put("Password", AESEncrypt(InputString, DB_PASSWORD_KEY));

    std::cout << "Input DB Name:" << std::endl;
    std::cin >> InputString;
    DBTree.put("DBName", InputString);

    boost::property_tree::ptree IniFileTree;
    IniFileTree.push_back(std::make_pair("Database", DBTree));
    boost::property_tree::write_ini(m_ConfigFileName, IniFileTree);

    std::cout << std::endl;
}

bool Config::LoadConfig()
{
    if (!LoadFileConfig())
    {
        g_Log.WriteError("Config: Read config file failed.");
        return false;
    }

    if (!LoadDatabaseConfig())
    {
        g_Log.WriteError("Config: Read database failed.");
        return false;
    }

    g_Log.WriteDebug("Config: Read config succeed.");
    return true;
}

bool Config::LoadFileConfig()
{
    try
    {
        boost::property_tree::ptree IniFileTree;
        boost::property_tree::read_ini(m_ConfigFileName, IniFileTree);

        const boost::property_tree::ptree &DBTree = IniFileTree.get_child("Database");
        m_DBPort = DBTree.get<int>("Port", 3306);
        m_DBHost = DBTree.get<std::string>("Host", "127.0.0.1");
        m_DBUser = DBTree.get<std::string>("User", "root");
        m_DBName = DBTree.get<std::string>("DBName", "jpc");

        const std::string &Password = DBTree.get<std::string>("Password", "root");
        m_DBPassword = AESDecrypt(Password, DB_PASSWORD_KEY);

        return true;

    }
    catch (...)
    {
        g_Log.WriteError("Config: boost::property_tree::read_ini throw error.");
        return false;
    }
}

bool Config::LoadDatabaseConfig()
{
    MySQLDatabase MySQL;
    if (!MySQL.Connect(m_DBHost.c_str(), m_DBUser.c_str(), m_DBPassword.c_str(), m_DBName.c_str(), m_DBPort))
    {
        g_Log.WriteError(BoostFormat("Config: Connnect config database failed, host = %s, user = %s, password = %s, name = %s, port = %d",
            m_DBHost.c_str(), m_DBUser.c_str(), m_DBPassword.c_str(), m_DBName.c_str(), m_DBPort));
        return false;
    }

    MySQLDataSet DataSet;
    std::string SQLString("SELECT * FROM server_config");
    if (!MySQL.ExecuteQuery(SQLString.c_str(), &DataSet))
    {
        g_Log.WriteError(BoostFormat("Config: Execute query failed: %s", SQLString.c_str()));
        return false;
    }

    int Section = 0;
    std::string ConfigName, ConfigValue;

    do
    {
        if (!DataSet.GetValue("config_name", ConfigName) ||
            !DataSet.GetValue("config_value", ConfigValue) ||
            !DataSet.GetValue("section", Section))
        {
            g_Log.WriteError("Config: Data set get value error.");
            continue;
        }

        switch(Section)
        {
        case 0:             ParseServerSection(ConfigName, ConfigValue);    break;
        case 1:             ParseDebugSection(ConfigName, ConfigValue);     break;
        default:            break;
        }

    } while (DataSet.MoveNext());

    MySQL.Disconnect();
    return true;
}

bool Config::ParseDebugSection(const std::string &ConfigName, const std::string &ConfigValue)
{
    if (ConfigName == "log_severity")
    {
        if (ConfigValue.size() != 1 || std::isdigit(ConfigValue[0]))
        {
            g_Log.WriteError(BoostFormat("Config: log severity value = %s error.", ConfigValue.c_str()));
            return false;
        }

        m_LogSeverity = atoi(ConfigValue.c_str());
        if (m_LogSeverity < LL_TEMP || m_LogSeverity > LL_ERROR)
        {
            g_Log.WriteError(BoostFormat("Config: log severity value = %s error.", ConfigValue.c_str()));
            return false;
        }

        g_Log.SetFilter(static_cast<LogLevel>(m_LogSeverity));
        return true;
    }

    return false;
}

bool Config::ParseServerSection(const std::string &ConfigName, const std::string &ConfigValue)
{
    if (ConfigName == "server_ip")
    {
        if (!ConfigValue.empty())
        {
            m_ServerIP = ConfigValue;
            return true;
        }

        std::vector<std::string> IPVector;
        if (!GetHostIPv4(IPVector))
        {
            g_Log.WriteError("Config: Can not get host IPv4 address.");
            return false;
        }

        for (std::vector<std::string>::size_type Index = 0; Index < IPVector.size(); Index++)
        {
            if (IPVector[Index].find("127") == std::string::npos)
            {
                m_ServerIP = IPVector[Index];
                return true;
            }
        }

        return false;
    }

    if (ConfigName == "smib_port")
    {
        m_SMIBPort = atoi(ConfigValue.c_str());
        return true;
    }

    if (ConfigName == "http_port")
    {
        m_HTTPPort = atoi(ConfigValue.c_str());
        return true;
    }

    return false;
}

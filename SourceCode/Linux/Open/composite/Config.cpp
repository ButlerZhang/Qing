#include "Config.h"
#include "../../LinuxTools.h"
#include "../../../Common/Boost/BoostLog.h"
#include "controller/SingleServer.h"
#include "core/tools/OpenSSLAES.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>



const int HTTP_DEFAULT_PORT = 8000;
const int SMIB_DEFAULT_PORT = 9000;

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



Config::Config() : m_ConfigFileName("project.ini")
{
    m_IsEnableLog = true;
    m_IsEnableHTTPS = true;
    m_IsLoadSucceed = false;
    m_LogSeverity = LL_DEBUG;
    m_SMIBPort = SMIB_DEFAULT_PORT;
    m_HTTPPort = HTTP_DEFAULT_PORT;

    g_Log.SetIsOkToWrite(m_IsEnableLog);
    event_set_log_callback(CallBack_LibEventLog);
}

Config::~Config()
{
}

void Config::GenerateConfigFile()
{
    g_Log.SetFilter(LL_ERROR);

    std::string InputString;
    boost::property_tree::ptree DBTree;
    std::cout << "Enter the mode to generate the configuration file." << std::endl;

    std::cout << std::endl << "Input database host IP:" << std::endl;
    std::cin >> InputString;
    DBTree.put("Host", InputString);

    std::cout << "Input database port:" << std::endl;
    std::cin >> InputString;
    DBTree.put("Port", InputString);

    std::cout << "Input database user:" << std::endl;
    std::cin >> InputString;
    DBTree.put("User", InputString);

    std::cout << "Input database password:" << std::endl;
    std::cin >> InputString;
    DBTree.put("Password", AESEncrypt(InputString, MY_AES_KEY));

    std::cout << "Input database name:" << std::endl;
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

    m_IsLoadSucceed = true;
    return m_IsLoadSucceed;
}

bool Config::LoadFileConfig()
{
    try
    {
        boost::property_tree::ptree IniFileTree;
        boost::property_tree::read_ini(m_ConfigFileName, IniFileTree);

        const boost::property_tree::ptree &DBTree = IniFileTree.get_child("Database");
        m_DBPort = DBTree.get<int>("Port", 3306);
        m_DBHost = DBTree.get<std::string>("Host", "localhost");
        m_DBUser = DBTree.get<std::string>("User", "root");
        m_DBName = DBTree.get<std::string>("DBName", "jpc");

        const std::string &Password = DBTree.get<std::string>("Password", "root");
        m_DBPassword = AESDecrypt(Password, MY_AES_KEY);

        return true;

    }
    catch (...)
    {
        g_Log.WriteError("Config: Read ini file throw error.");
        return false;
    }
}

bool Config::LoadDatabaseConfig()
{
    if (!g_SingleServer.GetDB().Isconnected() && !g_SingleServer.GetDB().Connect(m_DBHost.c_str(), m_DBUser.c_str(), m_DBPassword.c_str(), m_DBName.c_str(), m_DBPort))
    {
        g_Log.WriteError(BoostFormat("Config: Connnect database failed, host = %s, user = %s, password = %s, name = %s, port = %d",
            m_DBHost.c_str(), m_DBUser.c_str(), AESEncrypt(m_DBPassword, MY_AES_KEY).c_str(), m_DBName.c_str(), m_DBPort));

        return false;
    }

    return LoadServerConfigTable() && LoadDeviceJPCTable();
}

bool Config::LoadDeviceJPCTable()
{
    const std::vector<Ethernet::EthernetNode>& NodeVector = g_SingleServer.GetEthernet().GetNodeVector();
    for (std::vector<Ethernet::EthernetNode>::const_iterator it = NodeVector.begin(); it != NodeVector.end(); it++)
    {
        MySQLDataSet DataSet;
        const std::string &SQLString = BoostFormat("SELECT * FROM device_jpc WHERE ip = '%s' and mac = '%s'", it->m_IP.c_str(), it->m_MAC.c_str());
        if (!g_SingleServer.GetDB().ExecuteQuery(SQLString.c_str(), &DataSet))
        {
            g_Log.WriteError(BoostFormat("Config: Execute query failed: %s", SQLString.c_str()));
            return false;
        }

        if (DataSet.GetRecordCount() > 0 && DataSet.GetValue("port", m_SMIBPort))
        {
            g_Log.WriteDebug(BoostFormat("Config: load server ip = %s and port = %d succeed.", it->m_IP.c_str(), m_SMIBPort));
            m_ServerIP = it->m_IP;
            return true;
        }
    }

    m_SMIBPort = SMIB_DEFAULT_PORT;
    m_ServerIP = g_SingleServer.GetEthernet().GetLocalIP();

    std::string InsertSQL(BoostFormat("INSERT INTO device_jpc (name, mac, ip, port, subnet_mask, unreasonable_amount, last_update_time) VALUES('%s', '%s', '%s', %d, '%s', %d, %s)",
        "JPC", g_SingleServer.GetEthernet().GetLocalMAC().c_str(), m_ServerIP.c_str(), m_SMIBPort, g_SingleServer.GetEthernet().GetLocalMask().c_str(), 0, "Now()"));
    if (!g_SingleServer.GetDB().ExecuteQuery(InsertSQL.c_str()))
    {
        g_Log.WriteError(BoostFormat("Config: Execute query failed: %s", InsertSQL.c_str()));
        return false;
    }

    return true;
}

bool Config::LoadServerConfigTable()
{
    MySQLDataSet DataSet;
    std::string SQLString("SELECT * FROM server_config");
    if (!g_SingleServer.GetDB().ExecuteQuery(SQLString.c_str(), &DataSet))
    {
        g_Log.WriteError(BoostFormat("Config: Execute query failed: %s", SQLString.c_str()));
        return false;
    }

    if (DataSet.GetRecordCount() <= 0)
    {
        g_Log.WriteError("Config: server config table is empty.");
        return true;
    }

    int Section = 0; //not use yet
    std::string ConfigName, ConfigValue;

    do
    {
        if (!DataSet.GetValue("config_name", ConfigName) ||
            !DataSet.GetValue("config_value", ConfigValue) ||
            !DataSet.GetValue("section", Section))
        {
            continue;
        }

        ParseConfiguration(ConfigName, ConfigValue);

    } while (DataSet.MoveNext());

    return true;
}

bool Config::ParseConfiguration(const std::string &ConfigName, const std::string &ConfigValue)
{
    if (ConfigName == "log_severity")
    {
        if (ConfigValue.size() != 1 || !std::isdigit(ConfigValue[0]))
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

    if (ConfigName == "enable_https")
    {
        m_IsEnableHTTPS = !(ConfigValue.size() == 1 && std::isdigit(ConfigValue[0]) && atoi(ConfigValue.c_str()) == 0);
        return true;
    }

    if (ConfigName == "enable_log")
    {
        m_IsEnableLog = !(ConfigValue.size() == 1 && std::isdigit(ConfigValue[0]) && atoi(ConfigValue.c_str()) == 0);
        g_Log.SetIsOkToWrite(m_IsEnableLog);
        return true;
    }

    if (ConfigName == "http_port")
    {
        m_HTTPPort = atoi(ConfigValue.c_str());
        return true;
    }

    return false;
}

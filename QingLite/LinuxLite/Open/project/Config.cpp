#include "Config.h"
#include "../../LinuxTools.h"
#include "core/tools/BoostLog.h"
#include "core/database/MySQLDatabase.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>



Config g_Config;

Config::Config()
{
}

Config::~Config()
{
}

bool Config::LoadConfig(const std::string & FileName)
{
    if (!LoadFileConfig(FileName))
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

bool Config::LoadFileConfig(const std::string &FileName)
{
    boost::property_tree::ptree INITree;

    try
    {
        boost::property_tree::read_ini(FileName, INITree);
    }
    catch (...)
    {
        g_Log.WriteError("Config: boost::property_tree::read_ini throw error.");
        return false;
    }

    const boost::property_tree::ptree &DBTree = INITree.get_child("Database");
    m_DBPort = DBTree.get<int>("Port", 3306);
    m_DBHost = DBTree.get<std::string>("Host", "127.0.0.1");
    m_DBUser = DBTree.get<std::string>("User", "root");
    m_DBName = DBTree.get<std::string>("DBName", "jpc");
    m_DBPassword = DBTree.get<std::string>("Password", "root");

    return true;
}

bool Config::LoadDatabaseConfig()
{
    MySQLDatabase MySQL;
    if (!MySQL.Connect(m_DBHost.c_str(), m_DBUser.c_str(), m_DBPassword.c_str(), m_DBName.c_str(), m_DBPort))
    {
        g_Log.WriteError("Config: Connnect config database failed.");
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
        case 1:             //TODO
        default:            break;
        }

    } while (DataSet.MoveNext());

    MySQL.Disconnect();
    return true;
}

bool Config::ParseServerSection(const std::string & ConfigName, const std::string & ConfigValue)
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
            g_Log.WriteError("Can not get host IPv4 address.");
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

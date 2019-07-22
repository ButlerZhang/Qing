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
        return false;
    }

    if (!LoadDatabaseConfig())
    {
        return false;
    }

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
        BoostLog::WriteError("Config: Read config file error.");
        return false;
    }

    BoostLog::WriteDebug("Config: Read config file succeed.");

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
        BoostLog::WriteError("Config: Connnect config database failed.");
        return false;
    }

    MySQLDataSet DataSet;
    std::string SQLString("SELECT * FROM server_config");
    if (!MySQL.ExecuteQuery(SQLString.c_str(), &DataSet))
    {
        BoostLog::WriteError(BoostFormat("Config: Execute query failed: %s", SQLString.c_str()));
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
            BoostLog::WriteError("Config: Data set get value error.");
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
        m_ServerIP = ConfigValue;
        if (m_ServerIP.empty())
        {
            //m_ServerIP = GetHostIP();
        }

        return true;
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

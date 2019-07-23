#include "Config.h"
#include "../../LinuxTools.h"
#include "core/tools/BoostLog.h"
#include "core/tools/OpenSSLAES.h"
#include "core/database/MySQLDatabase.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>



Config g_Config;

Config::Config() : m_ConfigFileName("project.ini"), DB_PASSWORD_KEY("CJSZHCHCSZCJSZCJ")
{
}

Config::~Config()
{
}

void Config::EnterToolMode()
{
    bool IsLoop = true;
    std::string InputString;
    g_Log.WriteDebug("Enter tool mode.");

    while (IsLoop)
    {
        std::cout << std::endl << "Input select number: " << std::endl;
        std::cout << "1.Generate config file;" << std::endl;
        std::cout << "2.Encrypt string;" << std::endl;
        std::cout << "3.Decrypt string;" << std::endl;
        std::cout << "4.Exit program." << std::endl;

        std::cin >> InputString;
        if (InputString.size() != 1 || !std::isdigit(InputString[0]))
        {
            std::cout << "Please choice again:" << std::endl << std::endl;
            continue;
        }

        int Choice = atoi(InputString.c_str());
        switch (Choice)
        {
            case 1:
            {
                boost::property_tree::ptree DBTree;

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

                boost::property_tree::ptree INITree;
                INITree.push_back(std::make_pair("Database", DBTree));
                boost::property_tree::write_ini(m_ConfigFileName, INITree);

                std::cout << std::endl;
                IsLoop = false;
                break;
            }

        case 2:
            {
                std::cout << std::endl << "Input encrypt string:" << std::endl;
                std::cin >> InputString;

                const std::string &CipherText = AESEncrypt(InputString, DB_PASSWORD_KEY);
                std::cout << std::endl << "Encrypt result: " << CipherText << std::endl << std::endl;

                IsLoop = false;
                break;
            }

        case 3:
            {
                std::cout << std::endl << "Input decrypt string:" << std::endl;
                std::cin >> InputString;

                const std::string &ClearText = AESDecrypt(InputString, DB_PASSWORD_KEY);
                std::cout << std::endl << "Decrypt result: " << ClearText << std::endl << std::endl;

                IsLoop = false;
                break;
            }

        default:
            {
                std::cout << std::endl;
                IsLoop = false;
                break;
            }
        }
    }
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
    boost::property_tree::ptree INITree;

    try
    {
        boost::property_tree::read_ini(m_ConfigFileName, INITree);
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

    const std::string &Password = DBTree.get<std::string>("Password", "root");
    m_DBPassword = AESDecrypt(Password, DB_PASSWORD_KEY);

    return true;
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

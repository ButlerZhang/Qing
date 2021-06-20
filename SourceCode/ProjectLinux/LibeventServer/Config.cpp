#include "Config.h"
//#include "../../Linux/LinuxTools.h"
//#include "../../Share/Boost/BoostLog.h"
#include "controller/SingleServer.h"
#include "core/tools/OpenSSLAES.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>



const int HTTP_DEFAULT_PORT = 8000;
const int SMIB_DEFAULT_PORT = 9000;
const int HEARTBEAT_INTERNAL = 10;

const std::string SECTION_DATABASE("Database");
const std::string DATABASE_HOST("Host");
const std::string DATABASE_PORT("Port");
const std::string DATABASE_USER("User");
const std::string DATABASE_NAME("DBName");
const std::string DATABASE_PASSWORD("Password");

const std::string SECTION_SYSTEM("System");
const std::string SYSTEM_ENABLE_LOG("EnableLog");
const std::string SYSTEM_ENABLE_HTTPS("EnableHTTPS");
const std::string SYSTEM_ENABLE_DAEMON("EanbleDaemon");
const std::string SYSTEM_LOG_SEVERITY("LogSeverity");
const std::string SYSTEM_HTTP_PORT("HTTPPort");

const std::string SECTION_WATCHDOG("WatchDog");
const std::string WATCHDOG_ENABLE("EnableWatchDog");
const std::string WATCHDOG_DELETELOG("EnableDeleteLog");
const std::string WATCHDOG_HEARTBEAT("HeartbeatInternal");

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



Config::Config() : JPC_WATCHDOG("Qwatchdog"), JPC_SERVER("Qserver"), JPC_SMIB("Qclient"), m_IsLoadDBSucceed(false), m_ConfigFileName("config.ini")
{
    event_set_log_callback(CallBack_LibEventLog);

    //system
    m_IsEnableLog = true;
    m_IsEnableHTTPS = true;
    m_IsEnableDaemon = false;
    m_LogSeverity = LL_DEBUG;
    m_HTTPPort = HTTP_DEFAULT_PORT;
    m_SMIBPort = SMIB_DEFAULT_PORT;

    //watch dog
    m_IsEnableWatchDog = false;
    m_IsEnableDeleteLog = false;
    m_HeartbeatInternal = 0;
}

Config::~Config()
{
}

bool Config::CheckConfiguration()
{
    if (m_DBHost.empty())
    {
        g_Log.WriteError("Database host is empty.");
        return false;
    }

    if (m_DBUser.empty())
    {
        g_Log.WriteError("Database user is empty.");
        return false;
    }

    if (m_DBPassword.empty())
    {
        g_Log.WriteError("Database password is empty.");
        return false;
    }

    if (m_DBName.empty())
    {
        g_Log.WriteError("Database name is empty.");
        return false;
    }

    if (m_HTTPPort < 0 || m_HTTPPort > 65535)
    {
        g_Log.WriteError(BoostFormat("HTTP port = %d is wrong, reset it to %d", m_HTTPPort, HTTP_DEFAULT_PORT));
        m_HTTPPort = HTTP_DEFAULT_PORT;
    }

    if (m_HeartbeatInternal < 0 || m_HeartbeatInternal >(2 * 60 * 60)) //2 hour
    {
        g_Log.WriteError(BoostFormat("Heartbeat range is 1 to 7200 seconds, reset it to %d seconds.", HEARTBEAT_INTERNAL));
        m_HeartbeatInternal = HEARTBEAT_INTERNAL;
    }

    return true;
}

void Config::GenerateConfigFile()
{
    g_Log.SetFilter(LL_ERROR);

    std::string InputString;
    boost::property_tree::ptree DBTree;
    std::cout << "Enter the mode to generate the configuration file." << std::endl;

    std::cout << std::endl << "Input database host IP:" << std::endl;
    std::cin >> InputString;
    DBTree.put(DATABASE_HOST, InputString);

    std::cout << "Input database port:" << std::endl;
    std::cin >> InputString;
    DBTree.put(DATABASE_PORT, InputString);

    std::cout << "Input database user:" << std::endl;
    std::cin >> InputString;
    DBTree.put(DATABASE_USER, InputString);

    std::cout << "Input database password:" << std::endl;
    std::cin >> InputString;
    DBTree.put(DATABASE_PASSWORD, AESEncrypt(InputString, MY_AES_KEY));

    std::cout << "Input database name:" << std::endl;
    std::cin >> InputString;
    DBTree.put(DATABASE_NAME, InputString);

    boost::property_tree::ptree SystemTree;
    SystemTree.put(SYSTEM_ENABLE_LOG, 1);
    SystemTree.put(SYSTEM_ENABLE_HTTPS, 1);
    SystemTree.put(SYSTEM_ENABLE_DAEMON, 0);
    SystemTree.put(SYSTEM_LOG_SEVERITY, LL_DEBUG);
    SystemTree.put(SYSTEM_HTTP_PORT, HTTP_DEFAULT_PORT);

    boost::property_tree::ptree WatchDogTree;
    WatchDogTree.put(WATCHDOG_ENABLE, 0);
    WatchDogTree.put(WATCHDOG_DELETELOG, 0);
    WatchDogTree.put(WATCHDOG_HEARTBEAT, HEARTBEAT_INTERNAL);

    boost::property_tree::ptree IniFileTree;
    IniFileTree.push_back(std::make_pair(SECTION_DATABASE, DBTree));
    IniFileTree.push_back(std::make_pair(SECTION_SYSTEM, SystemTree));
    IniFileTree.push_back(std::make_pair(SECTION_WATCHDOG, WatchDogTree));

    boost::property_tree::write_ini(m_ConfigFileName, IniFileTree);
    std::cout << std::endl;
}

bool Config::LoadFileConfig()
{
    boost::property_tree::ptree IniFileTree;

    //step 1: read file
    try
    {
        boost::property_tree::read_ini(m_ConfigFileName, IniFileTree);
    }
    catch (...)
    {
        printf("Config: Can not read configuration file.\n");
        return false;
    }

    //step 2: system section
    try
    {
        const boost::property_tree::ptree &SystemTree = IniFileTree.get_child(SECTION_SYSTEM);
        m_IsEnableLog = SystemTree.get<bool>(SYSTEM_ENABLE_LOG, true);
        m_IsEnableHTTPS = SystemTree.get<bool>(SYSTEM_ENABLE_HTTPS, true);
        m_IsEnableDaemon = SystemTree.get<bool>(SYSTEM_ENABLE_DAEMON, false);
        m_HTTPPort = SystemTree.get<int>(SYSTEM_HTTP_PORT, HTTP_DEFAULT_PORT);
        m_LogSeverity = SystemTree.get<int>(SYSTEM_LOG_SEVERITY, LL_DEBUG);
        m_HeartbeatInternal = SystemTree.get<int>(WATCHDOG_HEARTBEAT, 30);

        g_Log.Initialize(m_ProcessName, std::string(), m_IsEnableLog);
        if (m_LogSeverity < LL_CORE || m_LogSeverity > LL_ERROR)
        {
            m_LogSeverity = LL_DEBUG;
        }
        g_Log.SetFilter(static_cast<LogLevel>(m_LogSeverity));
    }
    catch (...)
    {
        if (!g_Log.IsInitialize())
        {
            g_Log.Initialize(Config::JPC_SERVER, std::string(), true);
        }

        g_Log.WriteDebug("Config: Read system section failed, use default configuration.");
        //allow continue
    }

    //step 3: database section
    try
    {
        const boost::property_tree::ptree &DBTree = IniFileTree.get_child(SECTION_DATABASE);
        m_DBPort = DBTree.get<int>(DATABASE_PORT, 3306);
        m_DBHost = DBTree.get<std::string>(DATABASE_HOST, "localhost");
        m_DBUser = DBTree.get<std::string>(DATABASE_USER, "root");
        m_DBName = DBTree.get<std::string>(DATABASE_NAME, "jpc");
        m_DBPassword = AESDecrypt(DBTree.get<std::string>(DATABASE_PASSWORD, "root"), MY_AES_KEY);
    }
    catch (...)
    {
        g_Log.WriteError("Config: Read database section failed.");
        return false; //Database information must correct.
    }

    //step 4: watch dog section
    try
    {
        const boost::property_tree::ptree &WatchDogTree = IniFileTree.get_child(SECTION_WATCHDOG);
        m_IsEnableWatchDog = WatchDogTree.get<bool>(WATCHDOG_ENABLE, false);
        m_IsEnableDeleteLog = WatchDogTree.get<bool>(WATCHDOG_DELETELOG, false);
        m_HeartbeatInternal = WatchDogTree.get<int>(WATCHDOG_HEARTBEAT, 30);
    }
    catch (...)
    {
        g_Log.WriteDebug("Config: Read watch dog section failed, use default configuration.");
        //allow continue
    }

    return CheckConfiguration();
}

bool Config::LoadDatabaseConfig()
{
    if (!g_SingleServer.GetDB().Isconnected() && !g_SingleServer.GetDB().Connect(m_DBHost.c_str(), m_DBUser.c_str(), m_DBPassword.c_str(), m_DBName.c_str(), m_DBPort))
    {
        g_Log.WriteError(BoostFormat("Config: Connnect database failed, host = %s, user = %s, password = %s, name = %s, port = %d",
            m_DBHost.c_str(), m_DBUser.c_str(), AESEncrypt(m_DBPassword, MY_AES_KEY).c_str(), m_DBName.c_str(), m_DBPort));

        return false;
    }

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
            m_IsLoadDBSucceed = true;
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

    m_IsLoadDBSucceed = true;
    return true;
}

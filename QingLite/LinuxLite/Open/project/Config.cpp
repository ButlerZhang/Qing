#include "Config.h"



Config g_Config;

Config::Config()
{
    m_DBPort = 3306;
    m_DBHost = "192.168.3.126";
    m_DBUser = "root";
    m_DBName = "jpc";
    m_DBPassword = "root";
}

Config::~Config()
{
}

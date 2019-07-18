#include "DatabaseManager.h"
#include "../core/tools/BoostLog.h"



DatabaseManager g_DBManager;

DatabaseManager::DatabaseManager()
{
    m_HTTPDB = std::make_shared<MySQLDatabase>();
    m_SMIBDB = std::make_shared<MySQLDatabase>();
}

DatabaseManager::~DatabaseManager()
{
}

bool DatabaseManager::Start()
{
    const char *Host = "192.168.3.126";
    const char *User = "root";
    const char *Passwd = "root";
    const char *DB = "jpc";
    unsigned int Port = 3306;

    if (m_HTTPDB->Connect(Host, User, Passwd, DB, Port) == false)
    {
        BoostLog::WriteError("Connnect HTTP database failed.");
        return false;
    }

    if (m_SMIBDB->Connect(Host, User, Passwd, DB, Port) == false)
    {
        BoostLog::WriteError("Connnect SMIB database failed.");
        return false;
    }

    return true;
}

void DatabaseManager::CheckConnect()
{
    if (!m_HTTPDB->Isconnected())
    {
        BoostLog::WriteError("HTTP database is disconnected.");
        if (m_HTTPDB->Reconnect())
        {
            BoostLog::WriteDebug("HTTP database reconnect failed.");
        }
    }

    if (!m_SMIBDB->Isconnected())
    {
        BoostLog::WriteError("SMIB database is disconnected.");
        if (m_SMIBDB->Reconnect())
        {
            BoostLog::WriteDebug("SMIB database reconnect failed.");
        }
    }
}

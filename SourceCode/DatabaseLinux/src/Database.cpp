#include "Database.h"
#include "../../BoostLinux/src/BoostLog.h"
#include <stdlib.h>



DatabaseInfo::DatabaseInfo()
{
    m_DBPort = 0;
    m_TimeoutDays = 30;
}

bool DatabaseInfo::IsValid()
{
    return !m_DBHost.empty() && !m_DBUser.empty() && !m_DBPassword.empty() && !m_DBName.empty() && m_DBPort != 0;
}



DatabaseDataSet::DatabaseDataSet()
{
}

DatabaseDataSet::~DatabaseDataSet()
{
}

bool DatabaseDataSet::GetValue(const std::string &FieldName, std::string &Data) const
{
    return false;
}

bool DatabaseDataSet::GetValue(const std::string &FieldName, int64_t &Data) const
{
    std::string DataString;
    if (GetValue(FieldName, DataString) && !DataString.empty())
    {
        Data = atoll((const char*)DataString.c_str());
        return true;
    }

    g_Log.WriteError(BoostFormat("DatabaseDataSet get int64 value failed, filed name = %s", FieldName.c_str()));
    return false;
}

bool DatabaseDataSet::GetValue(const std::string &FieldName, int &Data) const
{
    std::string DataString;
    if (GetValue(FieldName, DataString) && !DataString.empty())
    {
        Data = atoi(DataString.c_str());
        return true;
    }

    g_Log.WriteError(BoostFormat("DatabaseDataSet get int value failed, filed name = %s", FieldName.c_str()));
    return false;
}

bool DatabaseDataSet::GetValue(const std::string &FieldName, double &Data) const
{
    std::string DataString;
    if (GetValue(FieldName, DataString) && !DataString.empty())
    {
        Data = atof(DataString.c_str());
        return true;
    }

    g_Log.WriteError(BoostFormat("DatabaseDataSet get double value failed, filed name = %s", FieldName.c_str()));
    return false;
}



Database::Database()
{
    m_Isconnected = false;
}

Database::~Database(void)
{
}

bool Database::Connect(const char *Host, const char *User, const char *Password, const char *DBName, unsigned int Port, const char *CharSet, int TimeoutDays)
{
    return false;
}

void Database::Disconnect()
{
}

bool Database::Isconnected()
{
    return m_Isconnected;
}

bool Database::Reconnect()
{
    return false;
}

bool Database::ExecuteQuery(const char* QueryStr, DatabaseDataSet * DataSet)
{
    return false;
}

#include "Database.h"

QING_NAMESPACE_BEGIN



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

bool DatabaseDataSet::GetValue(const std::wstring & FieldName, std::wstring & Data) const
{
    return false;
}

bool DatabaseDataSet::GetValue(const std::wstring & FieldName, __int64 & Data) const
{
    std::wstring DataString;
    if (GetValue(FieldName, DataString) && !DataString.empty())
    {
        Data = _wtoi64(DataString.c_str());
        return true;
    }

    return false;
}

bool DatabaseDataSet::GetValue(const std::wstring & FieldName, int & Data) const
{
    std::wstring DataString;
    if (GetValue(FieldName, DataString) && !DataString.empty())
    {
        Data = _wtoi(DataString.c_str());
        return true;
    }

    return false;
}

bool DatabaseDataSet::GetValue(const std::wstring & FieldName, double & Data) const
{
    std::wstring DataString;
    if (GetValue(FieldName, DataString) && !DataString.empty())
    {
        Data = _wtof(DataString.c_str());
        return true;
    }

    return false;
}



Database::Database()
{
    m_Isconnected = false;
}

Database::~Database(void)
{
}

bool Database::Connect(const wchar_t * Host, const wchar_t * User, const wchar_t * Password, const wchar_t * DBName, unsigned int Port, const wchar_t * CharSet, int TimeoutDays)
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

bool Database::ExecuteQuery(const wchar_t * QueryStr, DatabaseDataSet * DataSet)
{
    return false;
}

QING_NAMESPACE_END
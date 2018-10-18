#include "..\HeaderFiles\SQLiteDatabase.h"
#include "..\HeaderFiles\BoostLog.h"
#include "..\Reference\sqlite_3250200\sqlite3.h"

QING_NAMESPACE_BEGIN




SQLiteDataSet::SQLiteDataSet()
{
}

SQLiteDataSet::~SQLiteDataSet()
{
}

void SQLiteDataSet::Close()
{
}

bool SQLiteDataSet::MoveNext()
{
    m_CurrentIndex += m_Fields.size();
    return (m_CurrentIndex < m_Value.size());
}

bool SQLiteDataSet::GetValue(const std::string & FieldName, std::string & Data) const
{
    if (m_Fields.empty() || m_Value.empty() || m_CurrentIndex >= m_Value.size())
    {
        return false;
    }

    for (std::vector<std::string>::size_type Index = 0; Index < m_Fields.size(); ++Index)
    {
        if (m_Fields[Index] == FieldName)
        {
            Data = m_Value[m_CurrentIndex + Index];
            return true;
        }
    }

    return false;
}

int SQLiteDataSet::sqlite3_exec_callback(void * LiteDataSet, int argc, char ** argv, char ** ColName)
{
    SQLiteDataSet *DataSet = (SQLiteDataSet*)LiteDataSet;
    DataSet->m_CurrentIndex = 0;

    if (static_cast<int>(DataSet->m_Fields.size()) < argc)
    {
        for (int Index = 0; Index < argc; Index++)
        {
            DataSet->m_Fields.push_back(ColName[Index]);
            DataSet->m_Value.push_back(argv[Index]);
        }
    }
    else
    {
        for (int Index = 0; Index < argc; Index++)
        {
            DataSet->m_Value.push_back(argv[Index]);
        }
    }

    return 0;
}


SQLiteDatabase::SQLiteDatabase()
{
    m_sqlite = NULL;
    m_Isconnected = false;
}

SQLiteDatabase::~SQLiteDatabase(void)
{
    if (m_sqlite != NULL)
    {
        sqlite3_close(m_sqlite);
        m_sqlite = NULL;
    }
}

bool SQLiteDatabase::Connect(const char * Host, const char * User, const char * Password, const char * DBName, unsigned int Port, const char * CharSet, int TimeoutDays)
{
    if (sqlite3_open(DBName, &m_sqlite) != SQLITE_OK)
    {
        BoostLog::Write(LL_ERROR, "Can not open sqlite = %s, error = %s.",DBName, sqlite3_errmsg(m_sqlite));
        return false;
    }

    m_Isconnected = true;
    m_ConnectionInfo.m_DBName = DBName;

    return true;
}

void SQLiteDatabase::Disconnect()
{
    if (m_Isconnected)
    {
        sqlite3_close(m_sqlite);
        m_Isconnected = false;
    }
}

bool SQLiteDatabase::Isconnected()
{
    return m_Isconnected;
}

bool SQLiteDatabase::Reconnect()
{
    Disconnect();
    return Connect(NULL, NULL, NULL, m_ConnectionInfo.m_DBName.c_str(), 0);
}

bool SQLiteDatabase::ExecuteQuery(const char * QueryStr, DatabaseDataSet *DataSet)
{
    if (m_sqlite == NULL)
    {
        return false;
    }

    char * err_msg = NULL;
    if (DataSet == NULL)
    {
        if (sqlite3_exec(m_sqlite, QueryStr, NULL, NULL, &err_msg) != SQLITE_OK)
        {
            BoostLog::Write(LL_ERROR, "SQLite exec error = %s, querystring = %s", err_msg, QueryStr);
            return false;
        }
    }
    else
    {
        if (sqlite3_exec(m_sqlite, QueryStr, &SQLiteDataSet::sqlite3_exec_callback, DataSet, &err_msg) != SQLITE_OK)
        {
            BoostLog::Write(LL_ERROR, "SQLite exec error = %s, querystring = %s", err_msg, QueryStr);
            return false;
        }
    }

    return true;
}

QING_NAMESPACE_END



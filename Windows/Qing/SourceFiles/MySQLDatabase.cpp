#include "..\HeaderFiles\MySQLDatabase.h"
#include <Windows.h>
#include <mysql.h>
QING_NAMESPACE_BEGIN



MySQLDataSet::MySQLDataSet()
{
    m_ResultSet = NULL;
    m_Row = NULL;
}

MySQLDataSet::~MySQLDataSet()
{
    Close();
}

bool MySQLDataSet::Open(void* ResultSet)
{
    bool OpenResult = true;
    if (ResultSet != 0)
    {
        Close();

        m_ResultSet = (MYSQL_RES*)ResultSet;
        try
        {
            m_RecordCount = (unsigned long)mysql_num_rows(m_ResultSet);

            MYSQL_FIELD *Field = 0;
            while ((Field = mysql_fetch_field(m_ResultSet)) != 0)
            {
                m_Fields.push_back(Field->name);
            }

            m_Row = mysql_fetch_row(m_ResultSet);
        }
        catch (...)
        {
            OpenResult = false;
        }
    }

    return OpenResult;
}

void MySQLDataSet::Close()
{
    if (m_ResultSet != 0)
    {
        mysql_free_result(m_ResultSet);
        m_ResultSet = 0;
    }

    m_Fields.clear();
    m_RecordCount = 0;
    m_Row = 0;
}

bool MySQLDataSet::MoveNext()
{
    bool MoveResult = false;
    if (m_ResultSet != 0)
    {
        m_Row = mysql_fetch_row(m_ResultSet);
        MoveResult = (m_Row != 0);
    }

    return MoveResult;
}

bool MySQLDataSet::GetValue(const std::string & FieldName, std::string &Data) const
{
    if (m_ResultSet == NULL || m_Row == NULL)
    {
        return false;
    }

    for (std::vector<std::string>::size_type Index = 0; Index < m_Fields.size(); ++Index)
    {
        if (m_Fields[Index] == FieldName)
        {
            Data = m_Row[Index] ? m_Row[Index] : std::string();
            return true;
        }
    }

    return false;
}



MySQLDatabase::MySQLDatabase()
{
    m_Isconnected = false;
    m_MySQL = new MYSQL();
}

MySQLDatabase::~MySQLDatabase()
{
    delete m_MySQL;
    m_MySQL = NULL;
}

bool MySQLDatabase::Connect(const char * Host, const char * User, const char * Passwd, const char * DB, unsigned int Port, const char * CharSet, int TimeoutDays)
{
    Disconnect();

    if (mysql_init(m_MySQL) == 0)
    {
        return false;
    }

    if (mysql_real_connect(m_MySQL, Host, User, Passwd, DB, Port, 0, 0) == 0)
    {
        return false;
    }

    if (CharSet != 0)
    {
        mysql_set_character_set(m_MySQL, CharSet);
    }

    m_Isconnected = true;

    m_ConnectionInfo.m_DBName = DB;
    m_ConnectionInfo.m_DBHost = Host;
    m_ConnectionInfo.m_DBUser = User;
    m_ConnectionInfo.m_DBPassword = Passwd;
    m_ConnectionInfo.m_TimeoutDays = TimeoutDays;
    if (CharSet != 0)
    {
        m_ConnectionInfo.m_DBCharset = CharSet;
    }

    m_ConnectionInfo.m_DBPort = Port;

    std::string SQLString;
    const std::string &TimeString = std::to_string(TimeoutDays * 24 * 3600);

    SQLString = "SET interactive_timeout = " + TimeString;
    ExecuteQuery(SQLString.c_str());

    SQLString = "SET wait_timeout = " + TimeString;
    ExecuteQuery(SQLString.c_str());
    return true;
}

void MySQLDatabase::Disconnect()
{
    if (m_Isconnected)
    {
        mysql_close(m_MySQL);
        m_MySQL = NULL;
    }
}

bool MySQLDatabase::Isconnected()
{
    int PingResult = mysql_ping(m_MySQL);
    return m_Isconnected && PingResult == 0;
}

bool MySQLDatabase::Reconnect()
{
    if (m_ConnectionInfo.IsValid())
    {
        Disconnect();
        return Connect(m_ConnectionInfo.m_DBHost.c_str(),
            m_ConnectionInfo.m_DBUser.c_str(),
            m_ConnectionInfo.m_DBPassword.c_str(),
            m_ConnectionInfo.m_DBName.c_str(),
            m_ConnectionInfo.m_DBPort,
            m_ConnectionInfo.m_DBCharset.c_str(),
            m_ConnectionInfo.m_TimeoutDays);
    }

    return false;
}

bool MySQLDatabase::SetCharSet(const char * CharSet)
{
    if (m_Isconnected && CharSet != 0)
    {
        m_ConnectionInfo.m_DBCharset = CharSet;

        return mysql_set_character_set(m_MySQL, CharSet) == 0;
    }

    return false;
}

bool MySQLDatabase::ExecuteQuery(const char * QueryStr, DatabaseDataSet * MyDataSet)
{
    if (!m_Isconnected || QueryStr == 0)
    {
        return false;
    }

    if (mysql_query(m_MySQL, QueryStr) != 0)
    {
        return false;
    }

    if (MyDataSet != 0)
    {
        MYSQL_RES *TempResult = mysql_store_result(m_MySQL);
        return MyDataSet->Open(TempResult);
    }

    return true;
}

QING_NAMESPACE_END
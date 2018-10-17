#pragma once
#include "..\Model\Database\Database.h"
#include <vector>

typedef struct st_mysql MYSQL;
typedef struct st_mysql_field MYSQL_FIELD;
typedef struct st_mysql_res MYSQL_RES;
typedef char** MYSQL_ROW;

QING_NAMESPACE_BEGIN



class QING_DLL MySQLDataSet : public DatabaseDataSet
{
public:

    MySQLDataSet();
    virtual ~MySQLDataSet();

    virtual void Close();
    virtual bool MoveNext();
    virtual bool Open(void *ResultSet);
    virtual bool GetValue(const std::string &FieldName, std::string &Data) const;
    virtual bool GetValue(const std::string &FieldName, __int64 &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }
    virtual bool GetValue(const std::string &FieldName, int &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }
    virtual bool GetValue(const std::string &FieldName, double &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }

private:

    MYSQL_RES                  *m_ResultSet;
    MYSQL_ROW                   m_Row;
    std::vector<std::string>    m_Fields;
};



class QING_DLL MySQLDatabase : public Database
{
public:

    MySQLDatabase();
    ~MySQLDatabase();

    bool    Connect(const char *Host, const char *User, const char *Passwd, const char *DB, unsigned int Port, const char* CharSet = 0, int TimeoutDays = 30);
    void    Disconnect();
    bool    Isconnected();
    bool    Reconnect();

    bool    SetCharSet(const char* CharSet);
    bool    ExecuteQuery(const char* QueryStr, DatabaseDataSet *DataSet = NULL);

private:

    bool                                m_Isconnected;
    DatabaseInfo                        m_ConnectionInfo;
    MYSQL                              *m_MySQL;
};


QING_NAMESPACE_END
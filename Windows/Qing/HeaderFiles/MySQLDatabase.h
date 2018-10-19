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
    bool Open(MYSQL_RES* ResultSet);

    virtual void Close();
    virtual bool MoveNext();
    virtual unsigned long GetRecordCount() const { return m_RecordCount; }
    virtual bool GetValue(const std::wstring &FieldName, std::wstring &Data) const;
    virtual bool GetValue(const std::wstring &FieldName, __int64 &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }
    virtual bool GetValue(const std::wstring &FieldName, int &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }
    virtual bool GetValue(const std::wstring &FieldName, double &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }

private:

    unsigned long                    m_RecordCount;
    MYSQL_RES                       *m_ResultSet;
    MYSQL_ROW                        m_Row;
    std::vector<std::wstring>        m_FieldsVector;
};



class QING_DLL MySQLDatabase : public Database
{
public:

    MySQLDatabase();
    ~MySQLDatabase();

    bool    Connect(const wchar_t *Host, const wchar_t *User, const wchar_t *Passwd, const wchar_t *DB, unsigned int Port, const wchar_t* CharSet = 0, int TimeoutDays = 30);
    void    Disconnect();
    bool    Isconnected();
    bool    Reconnect();

    bool    SetCharSet(const wchar_t* CharSet);
    bool    ExecuteQuery(const wchar_t* QueryStr, DatabaseDataSet *DataSet = NULL);

private:

    MYSQL                              *m_MySQL;
};

QING_NAMESPACE_END
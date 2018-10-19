#pragma once
#include "..\Model\Database\Database.h"
#include <vector>

struct sqlite3;

QING_NAMESPACE_BEGIN



class QING_DLL SQLiteDataSet : public DatabaseDataSet
{
public:

    SQLiteDataSet();
    virtual ~SQLiteDataSet();

    virtual void Close();
    virtual bool MoveNext();
    virtual unsigned long GetRecordCount() const { return static_cast<unsigned long>(m_ValueVector.size()); }

    virtual bool GetValue(const std::wstring &FieldName, std::wstring &Data) const;
    virtual bool GetValue(const std::wstring &FieldName, __int64 &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }
    virtual bool GetValue(const std::wstring &FieldName, int &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }
    virtual bool GetValue(const std::wstring &FieldName, double &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }

    static int sqlite3_exec_callback(void *LiteDataSet, int argc, char** argv, char** ColName);

private:

    std::vector<std::wstring>::size_type     m_CurrentIndex;
    std::vector<std::wstring>                m_FieldsVector;
    std::vector<std::wstring>                m_ValueVector;
};


class QING_DLL SQLiteDatabase: public Database
{
public:

    SQLiteDatabase();
    ~SQLiteDatabase(void);

    bool Connect(const wchar_t *Host, const wchar_t *User, const wchar_t *Password, const wchar_t *DBName, unsigned int Port, const wchar_t* CharSet = 0, int TimeoutDays = 30);
    void Disconnect();
    bool Isconnected();
    bool Reconnect();
    bool ExecuteQuery(const wchar_t* QueryStr, DatabaseDataSet *DataSet = NULL);

private:

    sqlite3                            *m_sqlite;
};

QING_NAMESPACE_END
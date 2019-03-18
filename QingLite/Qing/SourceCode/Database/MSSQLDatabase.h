#pragma once
#include "Database.h"

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace \
    rename ("EOF", "adoEOF") \
    rename ("Connection", "adoConnection") \
    rename ("Command", "adoCommand")


QING_NAMESPACE_BEGIN



class QING_DLL MSSQLDataSet : public DatabaseDataSet
{
public:

    MSSQLDataSet();
    virtual ~MSSQLDataSet();
    bool Open(_ConnectionPtr ConnectionPtr, const wchar_t *QueryStr);

    virtual void Close();
    virtual bool MoveNext();
    virtual unsigned long GetRecordCount() const { return m_RecordsetPtr->GetRecordCount(); }

    virtual bool GetValue(const std::wstring &FieldName, std::wstring &Data) const;
    virtual bool GetValue(const std::wstring &FieldName, __int64 &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }
    virtual bool GetValue(const std::wstring &FieldName, int &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }
    virtual bool GetValue(const std::wstring &FieldName, double &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }

private:

    _RecordsetPtr        m_RecordsetPtr;
};



class QING_DLL MSSQLDatabase : public Database
{
public:

    MSSQLDatabase();
    ~MSSQLDatabase();

    bool    Connect(const wchar_t *Host, const wchar_t *User, const wchar_t *Passwd, const wchar_t *DB, unsigned int Port, const wchar_t* CharSet = 0, int TimeoutDays = 30);
    void    Disconnect();
    bool    Isconnected();
    bool    Reconnect();
    bool    ExecuteQuery(const wchar_t* QueryStr, DatabaseDataSet *DataSet = NULL);

private:

    _ConnectionPtr              m_ConnectionObject;
};

QING_NAMESPACE_END
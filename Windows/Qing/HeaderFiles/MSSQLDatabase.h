#pragma once
#include "..\Model\Database\Database.h"

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
    bool Open(_ConnectionPtr ConnectionPtr, const char *QueryStr);

    virtual void Close();
    virtual bool MoveNext();
    virtual unsigned long GetRecordCount() const { return m_RecordsetPtr->GetRecordCount(); }

    virtual bool GetValue(const std::string &FieldName, std::string &Data) const;
    virtual bool GetValue(const std::string &FieldName, __int64 &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }
    virtual bool GetValue(const std::string &FieldName, int &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }
    virtual bool GetValue(const std::string &FieldName, double &Data) const { return DatabaseDataSet::GetValue(FieldName, Data); }

private:

    _RecordsetPtr        m_RecordsetPtr;
};



class QING_DLL MSSQLDatabase : public Database
{
public:

    MSSQLDatabase();
    ~MSSQLDatabase();

    bool    Connect(const char *Host, const char *User, const char *Passwd, const char *DB, unsigned int Port, const char* CharSet = 0, int TimeoutDays = 30);
    void    Disconnect();
    bool    Isconnected();
    bool    Reconnect();
    bool    ExecuteQuery(const char* QueryStr, DatabaseDataSet *DataSet = NULL);

private:

    _ConnectionPtr              m_ConnectionObject;
};

QING_NAMESPACE_END
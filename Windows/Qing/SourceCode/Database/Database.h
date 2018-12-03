#pragma once
#include "..\QingBase.h"
#include <string>

QING_NAMESPACE_BEGIN



struct DatabaseInfo
{
    std::wstring     m_DBHost;
    std::wstring     m_DBUser;
    std::wstring     m_DBPassword;
    std::wstring     m_DBName;
    std::wstring     m_DBCharset;
    int              m_DBPort;
    int              m_TimeoutDays;

    DatabaseInfo();
    bool IsValid();
};



class QING_DLL DatabaseDataSet
{
public:

    DatabaseDataSet();
    virtual ~DatabaseDataSet();

    virtual void Close() { }
    virtual bool MoveNext() { return false; }
    virtual unsigned long GetRecordCount() const { return 0; }

    virtual bool GetValue(const std::wstring &FieldName, std::wstring &Data) const;
    virtual bool GetValue(const std::wstring &FieldName, __int64 &Data) const;
    virtual bool GetValue(const std::wstring &FieldName, int &Data) const;
    virtual bool GetValue(const std::wstring &FieldName, double &Data) const;
};



class QING_DLL Database
{
public:

    Database();
    ~Database(void);

    bool Connect(const wchar_t *Host, const wchar_t *User, const wchar_t *Password, const wchar_t *DBName, unsigned int Port, const wchar_t* CharSet = 0, int TimeoutDays = 30);
    void Disconnect();
    bool Isconnected();
    bool Reconnect();

    bool ExecuteQuery(const wchar_t* QueryStr, DatabaseDataSet *DataSet = NULL);

protected:

    bool                                m_Isconnected;
    DatabaseInfo                        m_ConnectionInfo;
};

QING_NAMESPACE_END
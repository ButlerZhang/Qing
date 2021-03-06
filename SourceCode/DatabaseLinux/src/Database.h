#pragma once
#include <string>



struct DatabaseInfo
{
    std::string      m_DBHost;
    std::string      m_DBUser;
    std::string      m_DBPassword;
    std::string      m_DBName;
    std::string      m_DBCharset;
    int              m_DBPort;
    int              m_TimeoutDays;

    DatabaseInfo();
    bool IsValid();
};



class DatabaseDataSet
{
public:

    DatabaseDataSet();
    virtual ~DatabaseDataSet();

    virtual void Close() { }
    virtual bool MoveNext() { return false; }
    virtual unsigned long GetRecordCount() const { return 0; }

    virtual bool GetValue(const std::string &FieldName, std::string &Data) const;
    virtual bool GetValue(const std::string &FieldName, int64_t &Data) const;
    virtual bool GetValue(const std::string &FieldName, int &Data) const;
    virtual bool GetValue(const std::string &FieldName, double &Data) const;
};



class Database
{
public:

    Database();
    ~Database(void);

    bool Connect(const char *Host, const char *User, const char *Password, const char *DBName, unsigned int Port, const char* CharSet = 0, int TimeoutDays = 30);
    void Disconnect();
    bool Isconnected();
    bool Reconnect();

    bool ExecuteQuery(const char* QueryStr, DatabaseDataSet *DataSet = NULL);

protected:

    bool                                m_Isconnected;
    DatabaseInfo                        m_ConnectionInfo;
};

#pragma once
#include "../core/database/MySQLDatabase.h"
#include <memory>



class DatabaseManager
{
public:

    DatabaseManager();
    ~DatabaseManager();

    bool Start();
    void CheckConnect();

    std::shared_ptr<MySQLDatabase>& GetSMIBDB() { return m_SMIBDB; }
    std::shared_ptr<MySQLDatabase>& GetHTTPDB() { return m_HTTPDB; }

private:

    std::shared_ptr<MySQLDatabase>    m_SMIBDB;
    std::shared_ptr<MySQLDatabase>    m_HTTPDB;
};

extern DatabaseManager g_DBManager;

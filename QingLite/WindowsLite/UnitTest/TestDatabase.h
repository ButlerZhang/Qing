#pragma once

#include <iostream>
#include "..\..\..\Qing\SourceCode\Boost\BoostLog.h"
#include "..\..\..\Qing\SourceCode\System\SystemShare.h"
#include "..\..\..\Qing\SourceCode\Database\MySQLDatabase.h"
#include "..\..\..\Qing\SourceCode\Database\MSSQLDatabase.h"
#include "..\..\..\Qing\SourceCode\Database\SQLiteDatabase.h"



void TestMySQLDatabase()
{
    Qing::MySQLDatabase MySQL;
    if (MySQL.Connect(L"127.0.0.1", L"root", L"cjgame", L"virtualsports", 3306, L"utf8"))
    {
        Qing::MySQLDataSet DataSet;
        if (MySQL.ExecuteQuery(L"SELECT * FROM vs_horse_racing_horse_names", &DataSet) && DataSet.GetRecordCount() > 0)
        {
            int RacerID;
            std::wstring RacerIDString;
            std::wstring EnglishName;
            std::wstring ChineseName;

            do
            {
                if (DataSet.GetValue(L"racer_name_id", RacerID) && 
                    DataSet.GetValue(L"english_name", EnglishName) && 
                    DataSet.GetValue(L"chinese_name", ChineseName))
                {
                    std::wstring LogString = std::to_wstring(RacerID) + L":" + EnglishName + L"," + ChineseName;
                    Qing::BoostLog::WriteInfo(LogString);
                    std::wcout << LogString << std::endl;
                }

            } while (DataSet.MoveNext());
        }
    }
}



void TestMSSQLDatabase()
{
    Qing::MSSQLDatabase MSSQL;
    if (MSSQL.Connect(L"192.168.3.19", L"sa", L"root", L"butler_virtualsports", 1433, L"utf8"))
    {
        Qing::MSSQLDataSet DataSet;
        if (MSSQL.ExecuteQuery(L"SELECT * FROM vs_horse_racing_horse_names", &DataSet) && DataSet.GetRecordCount() > 0)
        {
            int RacerID;
            std::wstring RacerIDString;
            std::wstring EnglishName;
            std::wstring ChineseName;

            do
            {
                if (DataSet.GetValue(L"racer_name_id", RacerID) &&
                    DataSet.GetValue(L"english_name", EnglishName) &&
                    DataSet.GetValue(L"chinese_name", ChineseName))
                {
                    std::wstring LogString = std::to_wstring(RacerID) + L":" + EnglishName + L"," + ChineseName;
                    Qing::BoostLog::WriteInfo(LogString);
                    std::wcout << LogString << std::endl;
                }

            } while (DataSet.MoveNext());
        }
    }
}



void TestSQLiteDatabase()
{
    Qing::SQLiteDatabase SQLite;
    if (SQLite.Connect(NULL , NULL, NULL, L"TestData\\virtualsports.sqlite", 0))
    {
        Qing::SQLiteDataSet DataSet;
        if (SQLite.ExecuteQuery(L"SELECT * FROM vs_horse_racing_horse_names", &DataSet) && DataSet.GetRecordCount() > 0)
        {
            int RacerID;
            std::wstring RacerIDString;
            std::wstring EnglishName;
            std::wstring ChineseName;

            do
            {
                if (DataSet.GetValue(L"racer_name_id", RacerID) &&
                    DataSet.GetValue(L"english_name", EnglishName) &&
                    DataSet.GetValue(L"chinese_name", ChineseName))
                {
                    std::wstring LogString = std::to_wstring(RacerID) + L":" + EnglishName + L"," + ChineseName;
                    Qing::BoostLog::WriteInfo(LogString);
                    std::wcout << LogString << std::endl;
                }

            } while (DataSet.MoveNext());
        }
    }
}
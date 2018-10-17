#pragma once

#include <iostream>
#include "..\..\..\Qing\HeaderFiles\BoostLog.h"
#include "..\..\..\Qing\HeaderFiles\CommonFunction.h"
#include "..\..\..\Qing\HeaderFiles\MySQLDatabase.h"



void TestMySQLDatabase()
{
    Qing::MySQLDatabase MySQL;
    if (MySQL.Connect("127.0.0.1", "root", "cjgame", "virtualsports", 3306, "utf8"))
    {
        Qing::MySQLDataSet DataSet;
        if (MySQL.ExecuteQuery("SELECT * FROM vs_horse_racing_horse_names", &DataSet) && DataSet.GetRecordCount() > 0)
        {
            int RacerID;
            std::string RacerIDString;
            std::string EnglishName;
            std::string ChineseName;

            do
            {
                if (DataSet.GetValue("racer_name_id", RacerID) && 
                    DataSet.GetValue("english_name", EnglishName) && 
                    DataSet.GetValue("chinese_name", ChineseName))
                {
                    std::string LogString = std::to_string(RacerID) + ":" + EnglishName + "," + ChineseName;
                    const std::wstring &TempLogString = Qing::StringToWString(LogString);

                    //Qing::BoostLog::Write(LogString, Qing::LL_ERROR);
                    std::wcout << TempLogString << std::endl;
                }

            } while (DataSet.MoveNext());
        }
    }
}
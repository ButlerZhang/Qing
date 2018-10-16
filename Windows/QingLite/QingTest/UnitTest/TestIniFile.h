#pragma once

#include <iostream>
#include "..\..\..\Qing\HeaderFiles\IniFile.h"



void TestIniFile()
{
    std::wstring DBSection(L"Database");
    std::wstring SystemSection(L"System");
    std::wstring FileName(L"TestData\\ServerConfig.ini");

    std::wstring DatabaseType(L"DatabaseType");
    std::wstring Host(L"Host");
    std::wstring Port(L"Port");
    std::wstring User(L"User");
    std::wstring Password(L"Password");
    std::wstring DBName(L"DBName");
    std::wstring EnableLog(L"EnableLog");
    std::wstring EnableTime(L"EnableTime");

    Qing::IniFile MyFile1;
    if (MyFile1.Open(FileName) && MyFile1.LoadData())
    {
        std::wcout << "FileSize = " << MyFile1.GetIniFileSize() << std::endl << std::endl;

        std::wcout << DatabaseType << "=" << MyFile1.GetInt(DBSection.c_str(), DatabaseType.c_str(), 0) << std::endl;
        std::wcout << Host << "=" << MyFile1.GetString(DBSection.c_str(), Host.c_str(), L"127.0.0.1") << std::endl;
        std::wcout << Port << "=" << MyFile1.GetInt(DBSection.c_str(), Port.c_str(), 0) << std::endl;
        std::wcout << User << "=" << MyFile1.GetString(DBSection.c_str(), User.c_str(), L"nobody") << std::endl;
        std::wcout << Password << "=" << MyFile1.GetString(DBSection.c_str(), Password.c_str(), Password.c_str()) << std::endl;
        std::wcout << DBName << "=" << MyFile1.GetString(DBSection.c_str(), DBName.c_str(), DBName.c_str()) << std::endl;
        std::wcout << EnableLog << "=" << MyFile1.GetString(SystemSection.c_str(), EnableLog.c_str(), EnableLog.c_str()) << std::endl << std::endl;

        std::wcout << std::endl << std::endl;
        MyFile1.WriteString(SystemSection.c_str(), EnableTime.c_str(), L"Y");
        MyFile1.WriteString(DBSection.c_str(), Host.c_str(), L"192.168.3.1");
    }

    Qing::IniFile MyFile2;
    if (MyFile2.Open(FileName) && MyFile2.LoadData())
    {
        std::wcout << "FileSize = " << MyFile1.GetIniFileSize() << std::endl << std::endl;

        std::wcout << DatabaseType << "=" << MyFile1.GetInt(DBSection.c_str(), DatabaseType.c_str(), 0) << std::endl;
        std::wcout << Host << "=" << MyFile1.GetString(DBSection.c_str(), Host.c_str(), L"127.0.0.1") << std::endl;
        std::wcout << Port << "=" << MyFile1.GetInt(DBSection.c_str(), Port.c_str(), 0) << std::endl;
        std::wcout << User << "=" << MyFile1.GetString(DBSection.c_str(), User.c_str(), L"nobody") << std::endl;
        std::wcout << Password << "=" << MyFile1.GetString(DBSection.c_str(), Password.c_str(), Password.c_str()) << std::endl;
        std::wcout << DBName << "=" << MyFile1.GetString(DBSection.c_str(), DBName.c_str(), DBName.c_str()) << std::endl;
        std::wcout << EnableLog << "=" << MyFile1.GetString(SystemSection.c_str(), EnableLog.c_str(), EnableLog.c_str()) << std::endl;
        std::wcout << EnableTime << "=" << MyFile1.GetString(SystemSection.c_str(), EnableTime.c_str(), EnableTime.c_str()) << std::endl << std::endl;
    }
}
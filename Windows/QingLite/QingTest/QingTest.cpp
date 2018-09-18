// QingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>

#include "..\..\Qing\HeaderFiles\QingLog.h"



int main()
{
    Qing::QingLog::DefaultInit();
    Qing::QingLog::SetFilter(Qing::QingLog::LL_DEBUG);
    
    for (int i = 0; i < 10000; i++)
    {
        Qing::QingLog::WriteUI("A ui severity message");
        Qing::QingLog::Write("A debug severity message", Qing::QingLog::LL_DEBUG);
        Qing::QingLog::Write("An informational severity message", Qing::QingLog::LL_INFO);
        Qing::QingLog::Write("An error severity message", Qing::QingLog::LL_ERROR);
    }

    Qing::QingLog::DefaultShutdown();

    std::cout << std::endl;
    system("pause");
    return 0;
}

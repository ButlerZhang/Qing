// QingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>

#include "..\..\Qing\HeaderFiles\QingLog.h"
#include "..\..\Qing\HeaderFiles\Utility.h"



int main()
{
    //Qing::QingLog::DefaultInit();
    //Qing::QingLog::SetFilter(Qing::QingLog::LL_DEBUG);

    Qing::QingLog::SetIsOkToWrite(true);
    if (Qing::QingLog::SetLogDirectory("C:\\LTGameAppData\\"))
    {
        Qing::QingLog::InitBaseSink("Active");
        Qing::QingLog::InitTemporarySink("UI");

        for (int i = 0; i < 10000; i++)
        {
            Qing::QingLog::Write("A temp severity message", Qing::QingLog::LL_TEMP);
            Qing::QingLog::Write("A debug severity message", Qing::QingLog::LL_DEBUG);
            Qing::QingLog::Write("An informational severity message", Qing::QingLog::LL_INFO);
            Qing::QingLog::Write("An error severity message", Qing::QingLog::LL_ERROR);
            Qing::QingLog::Write(Qing::QingLog::LL_TEMP, "My ID = %d, GUID = %s.", i, Qing::GetGUID().c_str());
        }
    }

    Qing::QingLog::DefaultShutdown();

    std::cout << std::endl;
    system("pause");
    return 0;
}

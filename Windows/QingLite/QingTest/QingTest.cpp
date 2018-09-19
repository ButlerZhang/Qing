// QingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>

#include "..\..\Qing\HeaderFiles\QingLog.h"
#include "..\..\Qing\HeaderFiles\Utility.h"
#include "..\..\Qing\HeaderFiles\RandomGenerator.h"

Qing::RandomGenerator g_random;



void HandlerThread()
{
    for (int i = 0; i < 10000; i++)
    {
        Qing::QingLog::Write("A temp severity message", Qing::QingLog::LL_TEMP);
        Qing::QingLog::Write("A debug severity message", Qing::QingLog::LL_DEBUG);
        Qing::QingLog::Write("An informational severity message", Qing::QingLog::LL_INFO);
        Qing::QingLog::Write("An error severity message", Qing::QingLog::LL_ERROR);

        Sleep(g_random.GetRandomUIntInRange(1, 10));
    }
}

void UIThread()
{
    static long long Counter = 0;
    for (int i = 0; i < 10000; i++)
    {
        Qing::QingLog::Write("A temp severity message", Qing::QingLog::LL_TEMP);
        Qing::QingLog::Write("A debug severity message", Qing::QingLog::LL_DEBUG);
        Qing::QingLog::Write(Qing::QingLog::LL_TEMP, "My ID = %d, GUID = %s.", Counter++, Qing::GetGUID().c_str());
        Sleep(g_random.GetRandomUIntInRange(1, 5));
    }
}


int main()
{
    //Qing::QingLog::DefaultInit();
    //Qing::QingLog::SetFilter(Qing::QingLog::LL_DEBUG);

    Qing::QingLog::SetIsOkToWrite(true);
    if (Qing::QingLog::SetLogDirectoryAutoAppendProgramName("C:\\QingLog"))
    {
        Qing::QingLog::InitBaseSink("Active");
        Qing::QingLog::InitTemporarySink("UI");

        std::thread handlerThread(HandlerThread);
        std::thread uiThread(UIThread);

        std::string quitString;
        while (std::cin >> quitString)
        {
            if (quitString == "q")
            {
                break;
            }

            Sleep(1000);
        }
    }

    Qing::QingLog::DefaultShutdown();

    std::cout << std::endl;
    system("pause");
    return 0;
}

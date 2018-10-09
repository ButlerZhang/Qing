// ServerLite.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Network\ServerNetwork.h"
#include "..\..\Qing\HeaderFiles\QingLog.h"
#include <iostream>



int main()
{
    //Qing::QingLog::DefaultInit();
    Qing::StartupNetwork();

    ServerNetwork MyServer;
    MyServer.Start(true);

    std::string quitstring;
    while (std::cin >> quitstring)
    {
        if (quitstring == "q" || quitstring == "quit")
        {
            MyServer.Stop();
            Sleep(1000);
            break;
        }

        Sleep(1000);
    }

    Qing::ShutdownNetwork();
    system("pause");
    return 0;
}


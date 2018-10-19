// ServerLite.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Network\ServerNetwork.h"
#include "..\..\Qing\HeaderFiles\BoostLog.h"
#include <iostream>

int main()
{
    //Qing::BoostLog::DefaultInit();

    ServerNetwork MyServer;
    MyServer.Start(std::string(), 12345, true);
    //MyServer.TestServer();

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

    system("pause");
    return 0;
}


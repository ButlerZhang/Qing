// ClientLite.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Network\ClientNetwork.h"
#include <iostream>



int main()
{
    //Qing::QingLog::DefaultInit();
    Qing::StartupNetwork();

    ClientNetwork MyClient;
    MyClient.Start(false);

    std::string quitstring;
    while (std::cin >> quitstring)
    {
        if (quitstring == "q" || quitstring == "quit")
        {
            MyClient.Stop();
            Sleep(1000);
            break;
        }

        Sleep(1000);
    }

    Qing::ShutdownNetwork();

    system("pause");
    return 0;
}

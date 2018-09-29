// ServerLite.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Network\ServerNetwork.h"
#include <iostream>



int main()
{
    Qing::StartupNetwork();

    ServerNetwork MyServer;
    MyServer.Start();

    std::string quitstring;
    while (std::cin >> quitstring)
    {
        if (quitstring == "q" || quitstring == "quit")
        {
            break;
        }

        Sleep(1000);
    }

    Qing::ShutdownNetwork();
    system("pause");
    return 0;
}


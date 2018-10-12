// ClientLite.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Network\ClientNetwork.h"
#include "..\..\Qing\HeaderFiles\QingLog.h"
#include <iostream>
#include <string>



int main()
{
    Qing::QingLog::DefaultInit();

    ClientNetwork MyClient(1000);
    MyClient.Start("127.0.0.1", 12345, false);
    MyClient.TestClient();

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

    system("pause");
    return 0;
}

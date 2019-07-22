#pragma once
#include "Config.h"
#include "controller/Client.h"
#include "controller/SingleServer.h"
#include "controller/MultiServer.h"
#include "controller/HTTPServer.h"
#include "core/tools/BoostLog.h"
#include <iostream>

int project(int argc, char *argv[])
{
    BoostLog::DefaultInit();

    if (argc <= 1 || atoi(argv[1]) == 0)
    {
        if (g_Config.LoadConfig("project.ini") == false)
        {
            return false;
        }

        g_HTTPServer.Start(g_Config.m_ServerIP, g_Config.m_HTTPPort);
        g_SingleServer.Start(g_Config.m_ServerIP, g_Config.m_SMIBPort);
    }
    else
    {
        Client MyClient;
        //MyClient.Start(ServerPort);
        MyClient.Start("192.168.3.126", 9000);
    }

    std::cout << std::endl;
    return 0;
}

#pragma once
#include "Config.h"
#include "controller/Client.h"
#include "controller/SingleServer.h"
#include "controller/HTTPServer.h"
#include "core/tools/BoostLog.h"



int project(int argc, char *argv[])
{
    g_Log.WriteInfo("Program start.");

    if (argc <= 1)                      //server
    {
        if (g_Config.LoadConfig())
        {
            g_HTTPServer.Start(g_Config.m_ServerIP, g_Config.m_HTTPPort);
            g_SingleServer.Start(g_Config.m_ServerIP, g_Config.m_SMIBPort);
        }
    }
    else if (atoi(argv[1]) == 0)        //tool mode
    {
        g_Config.EnterToolMode();
    }
    else                                //test client
    {
        Client MyClient;
        //MyClient.Start(ServerPort);
        MyClient.Start("192.168.3.126", 9000);
    }

    g_Log.WriteInfo("Program stop.\n");
    return 0;
}

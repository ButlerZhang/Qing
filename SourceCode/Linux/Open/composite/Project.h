#pragma once
#include "Config.h"
#include "controller/Client.h"
#include "controller/SingleServer.h"
#include "controller/HTTPServer.h"
#include "../../../Common/Boost/BoostLog.h"



int project(int argc, char *argv[])
{
    if (argc <= 1)                      //server
    {
        if (g_Config.LoadConfig())
        {
            g_Log.WriteInfo("Program start.");
            g_HTTPServer.Start(g_Config.m_ServerIP, g_Config.m_HTTPPort);
            g_SingleServer.Start(g_Config.m_ServerIP, g_Config.m_SMIBPort);
        }
    }
    else if (atoi(argv[1]) == 0)        //tool mode
    {
        g_Log.WriteInfo("Program start.");
        g_Config.GenerateConfigFile();
    }
    else                                //test client
    {
        g_Log.WriteInfo("Program start.");
        int ClientCount = atoi(argv[1]);
        if (ClientCount > 1000)
        {
            ClientCount = 1000;
        }

        pid_t Child;
        for (int Count = 1; Count < ClientCount; Count++)
        {
            Child = fork();
            if (Child < 0) {
                g_Log.WriteError(BoostFormat("Fork failed, Count = %d", Count));
            }
            else if (Child > 0) {
                sleep(1);
            }
            else {
                break;
            }
        }

        Client MyClient(getpid());
        //MyClient.Start(9000);
        MyClient.Start("192.168.3.126", 9000);
    }

    g_Log.WriteInfo("Program stop.");
    return 0;
}

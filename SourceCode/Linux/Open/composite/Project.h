#pragma once
#include "Config.h"
#include "controller/Client.h"
#include "controller/SingleServer.h"
#include "core/tools/RestartManager.h"



int project(int argc, char *argv[])
{
    if (argc <= 1)                               //server
    {
        g_Log.WriteInfo("Program start.");
        g_SingleServer.Start();
        g_Log.WriteInfo("Program stop.");
    }
    else if (argc == 2)
    {
        int Param = atoi(argv[1]);
        if (Param == 0)
        {
            g_Config.GenerateConfigFile();      //generate config
        }
        else if (Param > 0)                     //Start multi clients
        {
            StartMultiClient(Param, std::string(), g_Config.m_SMIBPort);
        }
    }
    else if (argc == 3)                         //start single clients
    {
        Client MyClient(getpid());
        MyClient.Start(argv[1], atoi(argv[2]));
    }
    else if (argc == 4)                         //start multi client
    {
        StartMultiClient(atoi(argv[1]), argv[2], atoi(argv[3]));
    }
    else
    {
        g_Log.WriteError("Program parammeters are incorrect.");
    }

    return 0;
}

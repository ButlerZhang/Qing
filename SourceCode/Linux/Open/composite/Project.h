#pragma once
#include "Config.h"
#include "controller/Client.h"
#include "controller/WatchDog.h"
#include "controller/SingleServer.h"
#include "controller/RestartManager.h"
#include "../../../BoostLinux/src/BoostLog.h"
#include <sys/prctl.h>



/*
*1.a.out                                                 : launch server
*2.a.out 0                                               : launch config tool
*3.a.out 0 watchdog_pid                                  : launch server by watchdog
*4.a.out client_count server_ip server_port              : launch multi clients
*5.a.out server_ip server_port heartbeat server_pid      : launch watchdog by server
*/

int LaunchConfigTool()
{
    g_Config.GenerateConfigFile();
    return 0;
}

int LaunchServer(long WatchDogPID)
{
    g_Config.SetProcessName(g_Config.JPC_SERVER);
    if (!g_Config.LoadFileConfig())
    {
        return 0;
    }

    if (prctl(PR_SET_NAME, g_Config.JPC_SERVER.c_str(), NULL, NULL, NULL) < 0)
    {
        g_Log.WriteDebug(BoostFormat("Program rename = %s error = %s", g_Config.JPC_SERVER.c_str(), strerror(errno)));
        return 0;
    }

    if (g_Config.m_IsEnableDaemon && daemon(1, 0) < 0)
    {
        g_Log.WriteError(BoostFormat("%s daemon error = %s", g_Config.JPC_SERVER.c_str(), strerror(errno)));
    }

    g_Log.WriteInfo("Program start.");
    g_SingleServer.Start(WatchDogPID);
    g_Log.WriteInfo("Program stop.");
    return 0;
}

int LaunchClients(int ClientCount, const std::string &ServerIP, int Port)
{
    if (ClientCount == 1)
    {
        g_Config.SetProcessName(g_Config.JPC_SMIB);
        g_Log.Initialize(g_Config.JPC_SMIB, std::string());

        if (prctl(PR_SET_NAME, g_Config.JPC_SMIB.c_str(), NULL, NULL, NULL) < 0)
        {
            g_Log.WriteDebug(BoostFormat("Program rename = %s error = %s", g_Config.JPC_SMIB.c_str(), strerror(errno)));
            return 0;
        }

        Client MyClient(getpid());
        MyClient.Start(ServerIP, Port);
    }
    else
    {
        RestartManager Restart;
        Restart.StartMultiClient(ClientCount, ServerIP, Port);
    }

    return 0;
}

int LaunchWatchDog(const std::string &ServerIP, int Port, int Heartbeat, long ServerPID)
{
    g_Config.SetProcessName(g_Config.JPC_WATCHDOG);
    if (!g_Config.LoadFileConfig())
    {
        return 0;
    }

    if (prctl(PR_SET_NAME, g_Config.JPC_WATCHDOG.c_str(), NULL, NULL, NULL) < 0)
    {
        g_Log.WriteDebug(BoostFormat("Program rename = %s error = %s", g_Config.JPC_WATCHDOG.c_str(), strerror(errno)));
        return 0;
    }

    if (daemon(1, 0) < 0)
    {
        g_Log.WriteError(BoostFormat("%s daemon error = %s", g_Config.JPC_WATCHDOG.c_str(), strerror(errno)));
    }

    WatchDog MyDog;
    MyDog.Start(ServerIP, Port, Heartbeat, ServerPID);

    return 0;
}



int project(int argc, char *argv[])
{
    switch (argc)
    {
    case 1:     return LaunchServer(0);
    case 2:     return LaunchConfigTool();
    case 3:     return LaunchServer(atol(argv[2]));
    case 4:     return LaunchClients(atoi(argv[1]), argv[2], atoi(argv[3]));
    case 5:     return LaunchWatchDog(argv[1], atoi(argv[2]), atoi(argv[3]), atol(argv[4]));
    default: {printf("Program parammeters are incorrect.\n");   return 0; }
    }
}

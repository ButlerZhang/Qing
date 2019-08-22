#include "Ethernet.h"
#include "../../../../LinuxTools.h"
#include "../../../../../Common/Boost/BoostLog.h"
#include "../../../../../Common/Boost/BoostTools.h"
#include <wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/reboot.h>



inline bool RestartOperatingSystem()
{
    sync();

    if (reboot(RB_AUTOBOOT) < 0)
    {
        g_Log.WriteError(BoostFormat("Restart operating system error = %s", strerror(errno)));
        return false;
    }

    return true;
}

inline bool RestartServer()
{
    pid_t Child = fork();
    if (Child < 0)
    {
        g_Log.WriteError(BoostFormat("Restart server can not fork, error = %s.", strerror(errno)));
        return false;
    }
    else if (Child == 0)
    {
        if (execl(GetProgramFullPath().c_str(), GetProgramName(true).c_str(), NULL) < 0)
        {
            g_Log.WriteError(BoostFormat("Restart server execl error = %s.", strerror(errno)));
            return false;
        }
    }
    else
    {
        g_Log.WriteInfo("Restart server father program stop.");
    }

    return true;
}

inline bool StartMultiClient(int ClientCount, std::string ServerIP, int Port)
{
    g_Log.SetFilter(LL_ERROR);

    if (ServerIP.empty())
    {
        Ethernet MyEthernet;
        if (!MyEthernet.Initialize())
        {
            g_Log.WriteError("Start multi clients can not initialize ethernet.");
            return false;
        }

        ServerIP = MyEthernet.GetLocalIP();
    }

    if (!IsIPValid(ServerIP))
    {
        g_Log.WriteError(BoostFormat("Start multi clients server ip = %s is not valid.", ServerIP.c_str()));
        return false;
    }

    if (ClientCount <= 0)
    {
        ClientCount = 1;
    }
    else if (ClientCount > 1000)
    {
        ClientCount = 1000;
    }

    for (int Count = 0; Count < ClientCount; Count++)
    {
        pid_t Child = fork();
        if (Child < 0)
        {
            g_Log.WriteError(BoostFormat("Start multi clients can not fork, error = %s.", strerror(errno)));
            return false;
        }
        else if (Child == 0)
        {
            if (execl(GetProgramFullPath().c_str(), GetProgramName(true).c_str(), ServerIP.c_str(), std::to_string(Port).c_str(), NULL) < 0)
            {
                g_Log.WriteError(BoostFormat("Start multi clients execl error = %s.", strerror(errno)));
                return false;
            }
        }
        else
        {
            sleep(1);
        }
    }

    return true;
}
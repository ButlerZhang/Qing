#include "RestartManager.h"
#include "../../../../Common/Boost/BoostLog.h"
#include "../../../../Common/Boost/BoostTools.h"
#include "../../../LinuxTools.h"
#include "../core/tools/Ethernet.h"
#include "../Config.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/reboot.h>
#include <boost/date_time/posix_time/posix_time.hpp>



RestartManager::RestartManager()
{
    m_IsStartedTargetProcess = false;
    m_RestartTimeout = 0;
    m_TargetProcessID = 0;
}

RestartManager::~RestartManager()
{
}

void RestartManager::Initialize(int HeartbeatTime, long TargetProcessID)
{
    m_RestartTimeout = HeartbeatTime * 2;
    g_Log.WriteDebug(BoostFormat("Restart manager set restart time out = %d seconds.", m_RestartTimeout));

    if (TargetProcessID > 0)
    {
        SetTargetProcessID(TargetProcessID);
        UpdateLastHeartbeatTime();
    }
}

bool RestartManager::IsTimeOut()
{
    if (m_RestartTimeout <= 0)
    {
        g_Log.WriteError("Restart manager timeout is zero.");
        return false;
    }

    if (m_LastHeartbeatTime.empty())
    {
        g_Log.WriteError("Restart manager last update heartbeat time is emtpy.");
        return false;
    }

    boost::posix_time::ptime Start(boost::posix_time::from_iso_string(m_LastHeartbeatTime));
    boost::posix_time::ptime Stop = boost::posix_time::second_clock::local_time();
    boost::posix_time::millisec_posix_time_system_config::time_duration_type Elapse = Stop - Start;

    int64_t ElapseSeconds = Elapse.total_seconds();
    bool IsTimeOut = (ElapseSeconds - m_RestartTimeout) > 0;

    g_Log.WriteDebug(BoostFormat("Restart manager check time out, elapse = %lld, timeout = %d, IsTimeout = %s",
        ElapseSeconds, m_RestartTimeout, IsTimeOut ? "Yes" : "No"));

    return IsTimeOut;
}

void RestartManager::CloseParentFileDescriptor()
{
    int StartFD = 3; //ignore stdin/stdout/stderr
    int StopFD = static_cast<int>(sysconf(_SC_OPEN_MAX));

    for (int fd = StartFD; fd < StopFD; fd++)
    {
        close(fd);
    }
}

void RestartManager::CheckRestart()
{
    g_Log.WriteDebug("Restart manager enter check restart process.");
    if (m_TargetProcessID > 0)
    {
        m_IsStartedTargetProcess = true;
        g_Log.WriteDebug(BoostFormat("Restart manager already has target pid = %ld", m_TargetProcessID));
    }

    if (!m_IsStartedTargetProcess)
    {
        if (g_Config.GetProcessName() == g_Config.JPC_SERVER)
        {
            m_IsStartedTargetProcess = StartWatchDog(g_Config.m_ServerIP, g_Config.m_SMIBPort, g_Config.m_HeartbeatInternal, getpid());
            g_Log.WriteDebug("Restart manager start watchdog.");
            UpdateLastHeartbeatTime();
        }
        else
        {
            g_Log.WriteDebug("Restart manager restart jpc server.");
            m_IsStartedTargetProcess = RestartServer(getpid());
            UpdateLastHeartbeatTime();
        }
    }

    if (IsTimeOut())
    {
        if (m_TargetProcessID <= 0)
        {
            g_Log.WriteDebug("Restart manager response timeout and target pid is zero");
        }
        else
        {
            KillProcess(m_TargetProcessID);
        }

        m_TargetProcessID = 0;
        m_LastHeartbeatTime.clear();
        m_IsStartedTargetProcess = false;
    }

    g_Log.WriteDebug("Restart manager leave check restart process.");
}

void RestartManager::UpdateLastHeartbeatTime()
{
    m_LastHeartbeatTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
    g_Log.WriteDebug(BoostFormat("Restart manager update last heartbeat time = %s.", m_LastHeartbeatTime.c_str()));
}

void RestartManager::SetTargetProcessID(long TargetProcessID)
{
    if (m_TargetProcessID <= 0)
    {
        m_TargetProcessID = TargetProcessID;
        g_Log.WriteDebug(BoostFormat("Restart manager set target pid = %ld", m_TargetProcessID));
    }
    else if (m_TargetProcessID != TargetProcessID)
    {
        g_Log.WriteError(BoostFormat("Restart manager set target pid error, old = %ld, new = %ld", m_TargetProcessID, TargetProcessID));
        m_TargetProcessID = TargetProcessID;
    }
}

bool RestartManager::KillProcess(long PID)
{
    if (kill(static_cast<pid_t>(PID), SIGTERM) < 0)
    {
        g_Log.WriteError(BoostFormat("Restart manager kill pid = %ld error = %s", PID, strerror(errno)));
        return false;
    }
    else
    {
        g_Log.WriteError(BoostFormat("Restart manager kill pid = %ld succeed", PID));
        return true;
    }
}

bool RestartManager::RestartOperatingSystem()
{
    sync();

    if (reboot(RB_AUTOBOOT) < 0)
    {
        g_Log.WriteError(BoostFormat("Restart operating system error = %s", strerror(errno)));
        return false;
    }

    return true;
}

bool RestartManager::RestartServer(long WatchDogPID)
{
    signal(SIGCHLD, SIG_IGN);

    pid_t Child = fork();
    if (Child < 0)
    {
        g_Log.WriteError(BoostFormat("Restart server can not fork, error = %s.", strerror(errno)));
        return false;
    }
    else if (Child == 0)
    {
        CloseParentFileDescriptor();
        if (execl(GetProgramFullPath().c_str(), GetProgramName(true).c_str(), "0", std::to_string(WatchDogPID).c_str(), NULL) < 0)
        {
            g_Log.WriteError(BoostFormat("Restart server execl error = %s.", strerror(errno)));
            return false;
        }
    }

    return true;
}

bool RestartManager::StartMultiClient(int ClientCount, std::string ServerIP, int Port)
{
    signal(SIGCHLD, SIG_IGN);

    g_Log.Initialize("Launcher", std::string(), true);
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
            CloseParentFileDescriptor();
            if (execl(GetProgramFullPath().c_str(), GetProgramName(true).c_str(),
                "1", ServerIP.c_str(), std::to_string(Port).c_str(), NULL) < 0)
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

bool RestartManager::StartWatchDog(const std::string &ServerIP, int Port, int HeartbeatTime, long ServerPID)
{
    signal(SIGCHLD, SIG_IGN);

    pid_t Child = fork();
    if (Child < 0)
    {
        g_Log.WriteError(BoostFormat("Start watchdog can not fork, error = %s.", strerror(errno)));
        return false;
    }
    else if (Child == 0)
    {
        CloseParentFileDescriptor();
        if (execl(GetProgramFullPath().c_str(), GetProgramName(true).c_str(),
            ServerIP.c_str(), std::to_string(Port).c_str(), std::to_string(HeartbeatTime).c_str(), std::to_string(ServerPID).c_str(), NULL) < 0)
        {
            g_Log.WriteError(BoostFormat("Start watchdog execl error = %s.", strerror(errno)));
            return false;
        }
    }

    return true;
}

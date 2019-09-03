#pragma once
#include <string>



class RestartManager
{
public:

    RestartManager();
    ~RestartManager();
    void Initialize(int HeartbeatTime, long TargetProcessID);

    void CheckRestart();
    void UpdateLastHeartbeatTime();
    void SetTargetProcessID(long TargetProcessID);

    bool KillProcess(long PID);
    bool RestartOperatingSystem();
    bool RestartServer(long WatchDogPID);
    bool StartMultiClient(int ClientCount, std::string ServerIP, int Port);
    bool StartWatchDog(const std::string &ServerIP, int Port, int HeartbeatTime, long ServerPID);

private:

    bool IsTimeOut();
    void CloseParentFileDescriptor();

private:

    bool                    m_IsStartedTargetProcess;
    int                     m_RestartTimeout;
    long                    m_TargetProcessID;
    std::string             m_LastHeartbeatTime;
};

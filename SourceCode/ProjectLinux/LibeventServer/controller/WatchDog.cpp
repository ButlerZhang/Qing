#include "WatchDog.h"
#include "../Config.h"
#include "../../Share/Boost/BoostLog.h"
#include "../../LinuxTools.h"
#include "../message/CodedMessage.h"
#include "../message/project.pb.h"
#include <sys/vfs.h>
#include <sys/stat.h>
#include <boost/date_time/posix_time/posix_time.hpp>



WatchDog::WatchDog()
{
    m_ServerPort = 0;
    m_HeartbeatTime = 5;
    m_IsConnected = false;
    m_RecvDataBuffer = std::make_shared<EventDataBuffer>();
}

WatchDog::~WatchDog()
{
}

bool WatchDog::ProcessMessage(NetworkMessage &NetworkMsg)
{
    int MessageType = DecodeMessage(NetworkMsg.m_Message);
    if (MessageType != Project::MessageType::MT_RANDOM)
    {
        g_Log.WriteError(BoostFormat("WatchDog recv error message type = %d.", MessageType));
        return false;
    }

    Project::RandomMessage RandomMsg;
    if (!RandomMsg.ParseFromString(NetworkMsg.m_Message))
    {
        g_Log.WriteError("WatchDog random message parse failed.");
        return false;
    }

    m_RestartManager.UpdateLastHeartbeatTime();
    m_RestartManager.SetTargetProcessID(static_cast<long>(RandomMsg.senderprocessid()));
    g_Log.WriteDebug("Random response message:\n" + RandomMsg.DebugString());
    return true;
}

bool WatchDog::Start(const std::string &ServerIP, int Port, int HeartbeatTime, long ServerPID)
{
    if (m_EventBase.m_eventbase == NULL)
    {
        g_Log.WriteError("WatchDog event base is NULL.");
        return false;
    }

    if (!AddCheckoutTimer(HeartbeatTime) || !AddSignal())
    {
        return false;
    }

    m_ServerPort = Port;
    m_ServerIP = ServerIP;
    m_HeartbeatTime = HeartbeatTime;
    g_Log.WriteDebug(BoostFormat("Start info: server ip = %s, port = %d, heartbeat = %d, server pid = %ld",
        m_ServerIP.c_str(), m_ServerPort, m_HeartbeatTime, ServerPID));

    m_RestartManager.Initialize(m_HeartbeatTime, ServerPID);
    g_Log.WriteInfo(BoostFormat("WatchDog (%s:%d) start dispatch...", m_ServerIP.c_str(), m_ServerPort));
    event_base_dispatch(m_EventBase.m_eventbase);
    return true;
}

bool WatchDog::ProcessCheckout()
{
    //step 1: start server process
    m_RestartManager.CheckRestart();

    //step 2, check connection
    if (!m_IsConnected)
    {
        g_Log.WriteDebug("WatchDog try to connecte server...");
        return ConnectServer(m_ServerIP, m_ServerPort);
    }

    //step 3, send heartbeat msg
    SendHeartbeatMessage();

    //step 4, delete log file
    DeleteLogFile();

    return true;
}

bool WatchDog::DeleteLogFile()
{
    if (!g_Config.m_IsEnableDeleteLog)
    {
        g_Log.WriteDebug("WatchDog disable delete log.");
        return true;
    }

    if (g_Log.GetLogDirectory().empty())
    {
        g_Log.WriteDebug("WatchDog log directory is empty.");
        return true;
    }

    int64_t NeedToDeleteSize = CalculateDeleteSpaceSize(g_Log.GetLogDirectory());
    if (NeedToDeleteSize <= 0)
    {
        g_Log.WriteDebug("WatchDog log space is still enough.");
        return true;
    }

    std::multimap<int64_t, std::string> FileMap;
    if (!GetLogFileNeedToDelete(FileMap, g_Log.GetLogDirectory()))
    {
        g_Log.WriteError("WatchDog can not find log files.");
        return false;
    }

    struct stat StatBuffer;
    int64_t TotalDeleteSize = 0;
    for (std::map<int64_t, std::string>::const_iterator it = FileMap.begin(); it != FileMap.end(); it++)
    {
        const std::string FilePath(g_Log.GetLogDirectory() + it->second);
        if (stat(FilePath.c_str(), &StatBuffer) < 0)
        {
            g_Log.WriteError(BoostFormat("WatchDog stat file = %s, error = %s", it->second.c_str(), strerror(errno)));
            continue;
        }

        TotalDeleteSize += StatBuffer.st_size;
        if (remove(FilePath.c_str()) < 0)
        {
            g_Log.WriteError(BoostFormat("WatchDog remove file = %s, error = %s", it->second.c_str(), strerror(errno)));
            continue;
        }

        g_Log.WriteDebug(BoostFormat("WatchDog remove file = %s, size = %ld", it->second.c_str(), StatBuffer.st_size));
        if (TotalDeleteSize >= NeedToDeleteSize)
        {
            break;
        }
    }

    g_Log.WriteDebug(BoostFormat("WatchDog delete total size = %llu, need to delete size = %ld", TotalDeleteSize, NeedToDeleteSize));
    return true;
}

bool WatchDog::SendHeartbeatMessage()
{
    static int64_t Sequence = 0;
    Project::RandomMessage RandomMsg;
    RandomMsg.set_serversequence(-1);
    RandomMsg.set_clientsequence(++Sequence);
    RandomMsg.set_senderprocessid(getpid());
    RandomMsg.set_randomdescriptor("WatchDog send sequence = " + std::to_string(Sequence));

    Project::MessageHeader *Header = RandomMsg.mutable_header();
    Header->set_type(Project::MessageType::MT_RANDOM);
    Header->set_transmissionid(GetUUID());

    g_Log.WriteDebug("WatchDog send random message:\n" + RandomMsg.DebugString());

    const std::string &EncodeString = EncodeMessage(RandomMsg, Project::MessageType::MT_RANDOM);
    return Send((void*)EncodeString.c_str(), EncodeString.size());
}

bool WatchDog::Send(const void * Data, size_t Size)
{
    if (!m_IsConnected)
    {
        g_Log.WriteError("WatchDog can not send data, no connected.");
        return false;
    }

    if (m_IOBuffer == NULL || m_IOBuffer->m_bufferevent == NULL)
    {
        g_Log.WriteError("WatchDog can not send data, IO buffer is NULL.");
        return false;
    }

    if (bufferevent_write(m_IOBuffer->m_bufferevent, Data, Size) != 0)
    {
        g_Log.WriteError(BoostFormat("WatchDog send data failed, size = %d.", Size));
        return false;
    }

    static unsigned long long SendCount = 0;
    g_Log.WriteInfo(BoostFormat("WatchDog send succeed, size = %d, count = %llu.", Size, ++SendCount));
    return true;
}

bool WatchDog::ConnectServer(const std::string &ServerIP, int Port)
{
    if (m_IsConnected)
    {
        g_Log.WriteError("WatchDog re-connect server.");
        return true;
    }

    struct sockaddr_in ServerAddress;
    InitializeSocketAddress(ServerAddress, ServerIP, Port);

    m_IOBuffer = std::make_shared<EventIOBuffer>();
    m_IOBuffer->m_bufferevent = bufferevent_socket_new(m_EventBase.m_eventbase, -1, BEV_OPT_CLOSE_ON_FREE);
    if (m_IOBuffer->m_bufferevent == NULL)
    {
        g_Log.WriteError("WatchDog create bufferevent failed.");
        return false;
    }

    bufferevent_setcb(m_IOBuffer->m_bufferevent, CallBack_RecvFromServer, NULL, CallBack_ConnectEvent, this);
    bufferevent_enable(m_IOBuffer->m_bufferevent, EV_READ | EV_PERSIST);

    if (bufferevent_socket_connect(m_IOBuffer->m_bufferevent, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) != 0)
    {
        g_Log.WriteError("WatchDog bufferevent connect failed.");
        return false;
    }

    return true;
}

int64_t WatchDog::CalculateDeleteSpaceSize(const std::string &LogPath)
{
    struct statfs DiskSpaceInfo;
    if (statfs(LogPath.c_str(), &DiskSpaceInfo) < 0)
    {
        g_Log.WriteError(BoostFormat("WatchDog can not statfs path = %s, error = %s.", LogPath.c_str(), strerror(errno)));
        return 0;
    }

    unsigned long long BlockSize = DiskSpaceInfo.f_bsize;
    unsigned long long FreeSize = DiskSpaceInfo.f_bfree * BlockSize;
    unsigned long long TotalSize = DiskSpaceInfo.f_blocks * BlockSize;
    unsigned long long AvailableSize = DiskSpaceInfo.f_bavail * BlockSize;
    g_Log.WriteDebug(BoostFormat("Log space total size = %llu, free size = %llu, available size = %llu", TotalSize, FreeSize, AvailableSize));

    int64_t DeleteSize = g_Log.GetMinFreeSpaceSize() - AvailableSize;
    g_Log.WriteDebug(BoostFormat("Log min size = %lu, need to delete size = %llu", g_Log.GetMinFreeSpaceSize(), DeleteSize));

    return DeleteSize;
}

bool WatchDog::GetLogFileInUsing(std::vector<std::string> &UsingLogFile, const std::string &LogPath)
{
    std::vector<std::string> ShellResultVector;
    std::vector<std::string> SplitResultVector;

    std::string Command("lsof +d " + LogPath);
    if (!GetShellResult(Command, ShellResultVector))
    {
        g_Log.WriteError(BoostFormat("WatchDog can not shell = %s", Command.c_str()));
        return false;
    }

    for (std::vector<std::string>::size_type ResultIndex = 1; ResultIndex < ShellResultVector.size(); ResultIndex++)
    {
        if (SplitString(ShellResultVector[ResultIndex], SplitResultVector, " ") && SplitResultVector.size() == 9) //9 items
        {
            std::string FileName = SplitResultVector[SplitResultVector.size() - 1];
            FileName.erase(FileName.end() - 1); //delete space

            std::string::size_type StartIndex = FileName.find_last_of("/");
            if (StartIndex != std::string::npos)
            {
                std::string::size_type CharactorCount = FileName.size() - StartIndex;
                UsingLogFile.push_back(FileName.substr(StartIndex + 1, CharactorCount));
            }
        }
    }

    return !UsingLogFile.empty();
}

bool WatchDog::GetLogFileNeedToDelete(std::multimap<int64_t, std::string> &DeleteFileMap, const std::string &LogPath)
{
    std::vector<std::string> UsingLogFileVector;
    GetLogFileInUsing(UsingLogFileVector, LogPath);

    DIR *dp = opendir(LogPath.c_str());
    if (dp == NULL)
    {
        g_Log.WriteError(BoostFormat("WatchDog can not open directory = %s", LogPath.c_str()));
        return false;
    }

    const size_t FILE_NAME_MIN_SIZE = 4;
    boost::posix_time::ptime CurrentTime = boost::posix_time::second_clock::local_time();

    struct dirent *dirp;
    std::vector<std::string> SplitVector;
    while ((dirp = readdir(dp)) != NULL)
    {
        if (strlen(dirp->d_name) < FILE_NAME_MIN_SIZE)
        {
            //ignore '.' and '..'
            continue;
        }

        std::string FileName(dirp->d_name);
        if (std::find(UsingLogFileVector.begin(), UsingLogFileVector.end(), FileName) != UsingLogFileVector.end())
        {
            g_Log.WriteError(BoostFormat("This log file = %s is using.", FileName.c_str()));
            continue;
        }

        std::string::size_type StopIndex = FileName.find(".log");
        if (StopIndex == std::string::npos)
        {
            g_Log.WriteDebug("Not log file : " + FileName);
            DeleteFileMap.insert(std::pair<int64_t, std::string>(INT64_MIN, FileName));
            continue;
        }

        if (!SplitString(FileName.substr(0, StopIndex), SplitVector, "_") || SplitVector.size() != FILE_NAME_MIN_SIZE)
        {
            g_Log.WriteDebug("Log file name format is incorrect : "+ FileName);
            DeleteFileMap.insert(std::pair<int64_t, std::string>(INT64_MIN, FileName));
            continue;
        }

        std::string LogFileTime(SplitVector[1] + "T" + SplitVector[2]);
        boost::posix_time::ptime CreateTime(boost::posix_time::from_iso_string(LogFileTime));
        boost::posix_time::millisec_posix_time_system_config::time_duration_type Elapse = CreateTime - CurrentTime;
        DeleteFileMap.insert(std::pair<int64_t, std::string>(Elapse.total_seconds(), FileName));
    }

    closedir(dp);
    return !DeleteFileMap.empty();
}

void WatchDog::CallBack_ConnectEvent(bufferevent * bev, short Events, void * UserData)
{
    WatchDog *MyWatcher = (WatchDog*)UserData;
    if (Events & BEV_EVENT_READING)
    {
        MyWatcher->m_IsConnected = false;
        g_Log.WriteError("WatchDog error encountered while reading");
    }
    else if (Events & BEV_EVENT_WRITING)
    {
        MyWatcher->m_IsConnected = false;
        g_Log.WriteError("WatchDog error encountered while writing");
    }
    else if (Events & BEV_EVENT_EOF)
    {
        MyWatcher->m_IsConnected = false;
        g_Log.WriteInfo("WatchDog connection closed.");
    }
    else if (Events & BEV_EVENT_TIMEOUT)
    {
        g_Log.WriteError("WatchDog user specified timeout reached.");
    }
    else if (Events & BEV_EVENT_CONNECTED)
    {
        MyWatcher->m_IsConnected = true;
        g_Log.WriteInfo("WatchDog connected server succeed.");
    }
    else if (Events & BEV_EVENT_ERROR)
    {
        MyWatcher->m_IsConnected = false;
        g_Log.WriteError("WatchDog unrecoverable error encountered.");
    }
    else
    {
        MyWatcher->m_IsConnected = false;
        g_Log.WriteError(BoostFormat("WatchDog event = %d, unknow error.", Events));
    }
}

void WatchDog::CallBack_RecvFromServer(bufferevent * bev, void *UserData)
{
    g_Log.WriteDebug("Process recv data from server call back.");

    WatchDog *MyWatcher = (WatchDog*)UserData;
    MyWatcher->RecvData(0, MyWatcher->m_IOBuffer, MyWatcher->m_RecvDataBuffer);
}

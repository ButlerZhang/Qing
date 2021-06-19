#pragma once
#include "../core/network/NetworkBase.h"
#include "RestartManager.h"
#include <vector>



class WatchDog : public NetworkBase
{
public:

    WatchDog();
    virtual ~WatchDog();
    virtual bool ProcessCheckout();
    virtual bool ProcessMessage(NetworkMessage &NetworkMsg);

    bool Start(const std::string &ServerIP, int Port, int HeartbeatTime, long ServerPID);

private:

    bool DeleteLogFile();
    bool SendHeartbeatMessage();
    bool Send(const void *Data, size_t Size);
    bool ConnectServer(const std::string &ServerIP, int Port);
    int64_t CalculateDeleteSpaceSize(const std::string &LogPath);
    bool GetLogFileInUsing(std::vector<std::string> &UsingLogFile, const std::string &LogPath);
    bool GetLogFileNeedToDelete(std::multimap<int64_t, std::string> &DeleteFileMap, const std::string &LogPath);

private:

    static void CallBack_ConnectEvent(struct bufferevent *bev, short Events, void *UserData);
    static void CallBack_RecvFromServer(struct bufferevent *bev, void *UserData);

private:

    bool                                         m_IsConnected;
    int                                          m_HeartbeatTime;
    int                                          m_ServerPort;
    std::string                                  m_ServerIP;
    RestartManager                               m_RestartManager;
    std::shared_ptr<EventIOBuffer>               m_IOBuffer;
    std::shared_ptr<EventDataBuffer>             m_RecvDataBuffer;
};

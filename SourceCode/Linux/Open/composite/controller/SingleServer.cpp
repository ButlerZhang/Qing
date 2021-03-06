#include "SingleServer.h"
#include "../../../../BoostLinux/src/BoostLog.h"
#include "../../../LinuxTools.h"
#include "../core/network/ThreadNoticeQueue.h"
#include "../handler/tcphandler/TCPHandler.h"
#include "../message/CodedMessage.h"
#include "../message/project.pb.h"
#include "HTTPServer.h"
#include "../Config.h"



SingleServer::SingleServer()
{
    m_HandlerVector.push_back(std::make_shared<TCPHandler>());
}

SingleServer::~SingleServer()
{
    m_HandlerVector.clear();
}

bool SingleServer::Start(long WatchDogPID)
{
    m_RestartManager.Initialize(g_Config.m_HeartbeatInternal, WatchDogPID);
    g_Log.WriteDebug(BoostFormat("Single Server watch dog pid = %ld", WatchDogPID));
    return SingleEventBaseServer::Start();
}

bool SingleServer::ProcessCheckout()
{
    //step 1: load ethernet information
    if (!m_Ethernet.Initialize())
    {
        return false;
    }

    //step 2: load configuration information
    if (!g_Config.IsLoadDBSucceed())
    {
        if (!g_Config.LoadDatabaseConfig())
        {
            return false;
        }

        if (!CreateListener(g_Config.m_ServerIP, g_Config.m_SMIBPort))
        {
            if (m_Listener.m_listener == NULL)
            {
                m_RestartManager.KillProcess(getpid());
            }

            return false;
        }

        g_HTTPServer.Start(g_Config.m_ServerIP, g_Config.m_HTTPPort);
        return true;
    }

    //step 3: check watchdog
    if (g_Config.m_IsEnableWatchDog && g_Config.m_HeartbeatInternal > 0)
    {
        m_RestartManager.CheckRestart();
    }

    //step 4: check database connection
    if (!m_SMIBDB.Isconnected())
    {
        g_Log.WriteError("Single Server SMIB database is disconnected.");

        if (m_SMIBDB.Reconnect())
        {
            g_Log.WriteInfo("Single Server SMIB database reconnect succeed.");
        }
        else
        {
            g_Log.WriteDebug("Single Server SMIB database reconnect failed.");
        }
    }

    //step 5: flush log file.
    g_Log.Flush();
    return true;
}

bool SingleServer::ProcessConnected()
{
    g_Log.WriteDebug("Single Server process connected.");
    return true;
}

bool SingleServer::ProcessDisconnected()
{
    g_Log.WriteDebug("Single Server process disconnected.");
    return true;
}

bool SingleServer::ProcessThreadNoticeQueue()
{
    NetworkMessage NetworkMsg;
    if (!g_ThreadNoticeQueue.PopMessage(NetworkMsg))
    {
        g_Log.WriteDebug("Single Server process notice queue, pop message failed.");
        return false;
    }

    if (!HasClient())
    {
        g_Log.WriteDebug("Single Server process notice queue, no smib client.");
        return false;
    }

    g_Log.WriteDebug(BoostFormat("Single Server process notice queue: %s", NetworkMsg.m_Message.c_str()));
    return DispatchMessage(NetworkMsg);


}

bool SingleServer::ProcessMessage(NetworkMessage &NetworkMsg)
{
    NetworkMsg.m_MessageType = DecodeMessage(NetworkMsg.m_Message);
    return DispatchMessage(NetworkMsg);
}

bool SingleServer::SendMessage(int MessageType, const google::protobuf::Message & ProtobufMsg)
{
    const std::string &DataString = EncodeMessage(ProtobufMsg, MessageType);
    return Send(DataString.c_str(), DataString.size());
}

bool SingleServer::SendMessage(int MessageType, NetworkMessage &NetworkMsg, const google::protobuf::Message &ProtobufMsg)
{
    const std::string &DataString = EncodeMessage(ProtobufMsg, MessageType);
    return Send(NetworkMsg, DataString.c_str(), DataString.size());
}
bool SingleServer::DispatchMessage(NetworkMessage &NetworkMsg)
{
    for (std::vector<std::shared_ptr<TCPHandler>>::size_type Index = 0; Index < m_HandlerVector.size(); Index++)
    {
        if (m_HandlerVector[Index]->ProcessMessage(NetworkMsg))
        {
            return true;
        }
    }
    return false;
}
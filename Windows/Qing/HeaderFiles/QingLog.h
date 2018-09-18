#pragma once
#include "QingBase.h"
#include <string>

QING_NAMESPACE_BEGIN



class QING_DLL QingLog
{
public:

    enum LogLevel
    {
        LL_UI,
        LL_DEBUG,
        LL_INFO,
        LL_ERROR,
    };

public:

    static void DefaultInit();
    static void DefaultShutdown();
    static void SetFilter(LogLevel level = QingLog::LL_DEBUG);

    static void WriteUI(const std::string &LogString) { Write(LogString, LL_UI); }
    static void Write(const std::string &LogString, LogLevel level = QingLog::LL_INFO);

    static void SetIsOkToWrite(bool IsOkToLog) { m_IsOkToWrite = IsOkToLog; }
    static void SetLogDirectory(const std::string Directory) { m_LogDirectory = Directory; }

private:

    QingLog() {}
    ~QingLog() {}

    static auto CreateSink(const std::string &FileName);
    static void InitBaseSink(const std::string &LogFileName = "Qing");
    static void InitAdditionalSink(const std::string &LogFileName = "UI");

private:

    static bool                m_IsOkToWrite;
    static std::string         m_LogDirectory;
};

QING_NAMESPACE_END
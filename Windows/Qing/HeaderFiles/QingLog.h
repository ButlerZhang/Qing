#pragma once
#include "QingBase.h"
#include <string>

QING_NAMESPACE_BEGIN



class QING_DLL QingLog
{
public:

    enum LogLevel
    {
        LL_TEMP,
        LL_DEBUG,
        LL_INFO,
        LL_ERROR,
    };

public:

    static void DefaultInit();
    static void DefaultShutdown();

    static void SetFilter(LogLevel Level = QingLog::LL_DEBUG);
    static void InitBaseSink(const std::string &LogFileName = "Qing");
    static void InitTemporarySink(const std::string &LogFileName = "Temp");

    static void Write(LogLevel Level, const char *Format, ...);
    static void Write(const std::string &LogString, LogLevel Level = QingLog::LL_INFO);

    static bool SetLogDirectory(const std::string &Directory);
    static void SetIsOkToWrite(bool IsOkToLog) { m_IsOkToWrite = IsOkToLog; }

private:

    QingLog() {}
    ~QingLog() {}

    static auto CreateSink(const std::string &FileName);

private:

    static bool                m_IsOkToWrite;
    static std::string         m_LogDirectory;
};

QING_NAMESPACE_END
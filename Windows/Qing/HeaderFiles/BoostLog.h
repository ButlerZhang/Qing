#pragma once
#include "QingBase.h"
#include <string>

QING_NAMESPACE_BEGIN



enum QING_DLL LogLevel
{
    LL_TEMP,
    LL_DEBUG,
    LL_INFO,
    LL_ERROR,
};



class QING_DLL BoostLog
{
public:

    static void DefaultInit();
    static void DefaultShutdown();

    static void InitBaseSink(const std::string &LogFileName = "Qing");
    static void InitTemporarySink(const std::string &LogFileName = "Temp");

    static void WriteTemp(const std::string &LogString) { WriteLog(LL_TEMP, LogString); }
    static void WriteInfo(const std::string &LogString) { WriteLog(LL_INFO, LogString); }
    static void WriteDebug(const std::string &LogString) { WriteLog(LL_DEBUG, LogString); }
    static void WriteError(const std::string &LogString) { WriteLog(LL_ERROR, LogString); }

    static bool SetLogDirectory(const std::string &Directory);
    static bool SetLogDirectoryAutoAppendProgramName(const std::string &Directory);

    static void SetFilter(LogLevel Level = LL_DEBUG);
    static void SetIsOkToWrite(bool IsOkToLog) { m_IsOkToWrite = IsOkToLog; }

private:

    BoostLog() {}
    ~BoostLog() {}

    static auto CreateSink(const std::string &FileName);
    static void WriteLog(LogLevel Level, const std::string &LogString);

private:

    static bool                m_IsOkToWrite;
    static std::string         m_LogDirectory;
};

QING_NAMESPACE_END
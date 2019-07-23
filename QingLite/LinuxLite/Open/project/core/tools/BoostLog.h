#pragma once
#include "BoostFormat.h"
#include <boost/log/sinks.hpp>

typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> TextSink;



enum LogLevel
{
    LL_TEMP,
    LL_DEBUG,
    LL_INFO,
    LL_ERROR,
};

class BoostLog
{
public:

    BoostLog();
    ~BoostLog();

    void InitBaseSink(const std::string &LogFileName = "Boost");
    void InitTemporarySink(const std::string &LogFileName = "Temp");

    void WriteTemp(const std::string &LogString) { WriteLog(LL_TEMP, LogString); }
    void WriteInfo(const std::string &LogString) { WriteLog(LL_INFO, LogString); }
    void WriteDebug(const std::string &LogString) { WriteLog(LL_DEBUG, LogString); }
    void WriteError(const std::string &LogString) { WriteLog(LL_ERROR, LogString); }

    void SetFilter(LogLevel Level = LL_DEBUG);
    bool SetLogDirectory(const std::string &Directory);
    void SetIsOkToWrite(bool IsOkToLog) { m_IsOkToWrite = IsOkToLog; }

private:

    boost::shared_ptr<TextSink> CreateSink(const std::string &FileName);
    void WriteLog(LogLevel Level, const std::string &LogString);

private:

    bool                m_IsOkToWrite;
    std::string         m_LogDirectory;
};

extern BoostLog g_Log;

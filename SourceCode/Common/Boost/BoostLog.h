#pragma once
#include "BoostFormat.h"
#include <boost/log/sinks.hpp>

typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> TextSink;
typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> ConsoleSink;



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
    void Initialize(const std::string &ProgramName, const std::string &WorkDirectory, bool IsOkToWrite = true);

    void WriteTemp(const std::string &LogString) { WriteLog(LL_TEMP, LogString); }
    void WriteInfo(const std::string &LogString) { WriteLog(LL_INFO, LogString); }
    void WriteDebug(const std::string &LogString) { WriteLog(LL_DEBUG, LogString); }
    void WriteError(const std::string &LogString) { WriteLog(LL_ERROR, LogString); }

    void Flush();
    void SetFilter(LogLevel Level = LL_DEBUG);
    bool SetLogDirectory(const std::string &Directory);
    void SetIsOkToWrite(bool IsOkToLog) { m_IsOkToWrite = IsOkToLog; }

    inline unsigned long GetRotationSize() const { return m_RotationSize; }
    inline unsigned long GetMinFreeSpaceSize() const { return m_MinFreeSpaceSize; }

    inline bool IsInitialize() const { return !m_LogDirectory.empty(); }
    inline const std::string& GetLogDirectory() const { return m_LogDirectory; }

private:

    boost::shared_ptr<TextSink> CreateSink(const std::string &FileName);
    void WriteLog(LogLevel Level, const std::string &LogString);

private:

    const unsigned long           m_RotationSize;
    const unsigned long           m_MinFreeSpaceSize;

    bool                          m_IsOkToWrite;
    std::string                   m_LogDirectory;
};

extern BoostLog g_Log;

#pragma once
#include "QingBase.h"
#include <string>

QING_NAMESPACE_BEGIN



class QING_DLL QingLog
{
public:

    enum LogLevel
    {
        LL_DEBUG,
        LL_INFO,
        LL_ERROR,
        LL_FATAL,
        LL_REPORT,
    };

public:

    static void UnitTest();
    static QingLog& GetInstance();

    void SetIsOkToWrite(bool IsOkToLog) { m_IsOkToWrite = IsOkToLog; }
    void SetLogDirectory(const std::string Directory) { m_LogDirectory = Directory; }
    void Write(const std::string &LogString, LogLevel level = QingLog::LL_DEBUG) const;

private:

    QingLog();
    ~QingLog();

    auto GetSinkBackend(const std::string &FileName) const;
    void InitBaseSink(const std::string &LogFileName = "Qing") const;
    void InitAdditionalSink(const std::string &LogFileName = "QingReport") const;

private:

    bool                m_IsOkToWrite;
    std::string         m_LogDirectory;
};

QING_NAMESPACE_END
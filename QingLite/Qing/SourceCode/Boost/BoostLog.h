#pragma once
#include "..\QingBase.h"
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

    static void InitBaseSink(const std::wstring &LogFileName = L"Qing");
    static void InitTemporarySink(const std::wstring &LogFileName = L"Temp");

    static void WriteTemp(const std::wstring &LogString) { WriteLog(LL_TEMP, LogString); }
    static void WriteInfo(const std::wstring &LogString) { WriteLog(LL_INFO, LogString); }
    static void WriteDebug(const std::wstring &LogString) { WriteLog(LL_DEBUG, LogString); }
    static void WriteError(const std::wstring &LogString) { WriteLog(LL_ERROR, LogString); }

    static bool SetLogDirectory(const std::wstring &Directory);
    static bool SetLogDirectoryAutoAppendProgramName(const std::wstring &Directory);

    static void SetFilter(LogLevel Level = LL_DEBUG);
    static void SetIsOkToWrite(bool IsOkToLog) { m_IsOkToWrite = IsOkToLog; }

private:

    BoostLog() {}
    ~BoostLog() {}

    static auto CreateSink(const std::wstring &FileName);
    static void WriteLog(LogLevel Level, const std::wstring &LogString);

private:

    static bool                m_IsOkToWrite;
    static std::wstring         m_LogDirectory;
};

QING_NAMESPACE_END
#include "..\HeaderFiles\BoostLog.h"
#include "..\HeaderFiles\CommonFunction.h"

#include <Windows.h>
#include <boost/log/sinks.hpp>
#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

QING_NAMESPACE_BEGIN

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(BoostLogger, src::severity_logger_mt<LogLevel>)
typedef sinks::synchronous_sink<sinks::text_file_backend> TextSink;

bool BoostLog::m_IsOkToWrite = true;
std::string BoostLog::m_LogDirectory;


// The formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm, LogLevel lvl)
{
    switch (lvl)
    {
    case Qing::LL_TEMP:         strm << "TEMP ";                break;
    case Qing::LL_DEBUG:        strm << "DEBUG";                break;
    case Qing::LL_INFO:         strm << "INFO ";                break;
    case Qing::LL_ERROR:        strm << "ERROR";                break;
    default:                    strm << std::to_string(lvl);    break;
    }

    return strm;
}



void BoostLog::DefaultInit()
{
    SetIsOkToWrite(true);
    SetLogDirectoryAutoAppendProgramName("C:\\QingLog\\");

    InitBaseSink();
    InitTemporarySink();
}

void BoostLog::DefaultShutdown()
{
    logging::core::get()->remove_all_sinks();
}

void BoostLog::SetFilter(LogLevel Level)
{
    logging::core::get()->set_filter(expr::attr<LogLevel>("Severity") >= Level);
}

auto BoostLog::CreateSink(const std::string & FileName)
{
    const size_t ONE_MB = 1024 * 1024;

    boost::shared_ptr<sinks::text_file_backend> Backend = boost::make_shared<sinks::text_file_backend>(
        keywords::file_name = m_LogDirectory + FileName + "_%Y-%m-%d_%H-%M-%S.log",
        keywords::rotation_size = 100 * ONE_MB,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::min_free_space = 500 * ONE_MB);

    Backend->auto_flush(true);
    boost::shared_ptr<TextSink> NewSink(new TextSink(Backend));

    NewSink->set_formatter(
        expr::format("[%1%][%2%][%3%]: %4%")
        % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
        % expr::attr<LogLevel>("Severity")
        % expr::attr<attrs::current_thread_id::value_type>("ThreadID")
        % expr::smessage
    );

    return NewSink;
}

void BoostLog::InitBaseSink(const std::string &LogFileName)
{
    boost::shared_ptr<TextSink> BaseSink = CreateSink(LogFileName);
    BaseSink->set_filter(expr::attr<LogLevel>("Severity") >= LL_DEBUG);

    logging::core::get()->add_sink(BaseSink);
    logging::add_common_attributes();
}

void BoostLog::InitTemporarySink(const std::string & LogFileName)
{
    boost::shared_ptr<TextSink> TempSink = CreateSink(LogFileName);
    TempSink->set_filter(expr::attr<LogLevel>("Severity") == LL_TEMP);

    logging::core::get()->add_sink(TempSink);
    logging::add_common_attributes();
}

//void BoostLog::Write(LogLevel Level, const char * Format, ...)
//{
//    if (m_IsOkToWrite && Format != NULL)
//    {
//        const size_t LOGBUFFERSIZE = 2048;
//
//        va_list VaList = NULL;
//        va_start(VaList, Format);
//
//        size_t FormatSize = _vscprintf(Format, VaList) + 1;
//        size_t CopySize = max(FormatSize, LOGBUFFERSIZE);
//
//        char LogString[LOGBUFFERSIZE + 1];
//        memset(LogString, 0, sizeof(LogString));
//
//        _vsnprintf_s(LogString, LOGBUFFERSIZE, CopySize, Format, VaList);
//        va_end(VaList);
//
//        WriteLog(Level, LogString);
//    }
//}

void BoostLog::WriteLog(LogLevel Level, const std::string &LogString)
{
    if (m_IsOkToWrite)
    {
        //src::severity_logger_mt<BoostLog::LogLevel>& lg = BoostLogger::get();
        BOOST_LOG_SEV(BoostLogger::get(), Level) << LogString;
    }
}

bool BoostLog::SetLogDirectory(const std::string &Directory)
{
    if ((GetFileAttributesA(Directory.c_str()) & FILE_ATTRIBUTE_DIRECTORY) > 0)
    {
        m_LogDirectory = Directory;
        if (m_LogDirectory[m_LogDirectory.size() - 1] != '\\')
        {
            m_LogDirectory.append("\\");
        }

        return true;
    }

    return false;
}

bool BoostLog::SetLogDirectoryAutoAppendProgramName(const std::string &Directory)
{
    if (SetLogDirectory(Directory))
    {
        m_LogDirectory.append(GetProgramName());
        m_LogDirectory.append("\\");
        return true;
    }

    return false;
}

QING_NAMESPACE_END
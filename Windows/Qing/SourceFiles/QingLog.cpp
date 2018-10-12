#include "..\HeaderFiles\QingLog.h"
#include "..\HeaderFiles\Utility.h"

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

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(QingLogger, src::severity_logger_mt<LogLevel>)
typedef sinks::synchronous_sink<sinks::text_file_backend> TextSink;

bool QingLog::m_IsOkToWrite = true;
std::string QingLog::m_LogDirectory;



void QingLog::DefaultInit()
{
    SetIsOkToWrite(true);
    SetLogDirectoryAutoAppendProgramName("C:\\QingLog\\");

    InitBaseSink();
    InitTemporarySink();
}

void QingLog::DefaultShutdown()
{
    logging::core::get()->remove_all_sinks();
}

void QingLog::SetFilter(LogLevel Level)
{
    logging::core::get()->set_filter(expr::attr<LogLevel>("Severity") >= Level);
}

auto QingLog::CreateSink(const std::string & FileName)
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

void QingLog::InitBaseSink(const std::string &LogFileName)
{
    boost::shared_ptr<TextSink> BaseSink = CreateSink(LogFileName);
    BaseSink->set_filter(expr::attr<LogLevel>("Severity") >= LL_DEBUG);

    logging::core::get()->add_sink(BaseSink);
    logging::add_common_attributes();
}

void QingLog::InitTemporarySink(const std::string & LogFileName)
{
    boost::shared_ptr<TextSink> TempSink = CreateSink(LogFileName);
    TempSink->set_filter(expr::attr<LogLevel>("Severity") == LL_TEMP);

    logging::core::get()->add_sink(TempSink);
    logging::add_common_attributes();
}

void QingLog::Write(LogLevel Level, const char * Format, ...)
{
    if (Format != NULL)
    {
        const size_t LOGBUFFERSIZE = 2048;

        va_list VaList = NULL;
        va_start(VaList, Format);

        size_t FormatSize = _vscprintf(Format, VaList) + 1;
        size_t CopySize = max(FormatSize, LOGBUFFERSIZE);

        char LogString[LOGBUFFERSIZE + 1];
        memset(LogString, 0, sizeof(LogString));

        _vsnprintf_s(LogString, LOGBUFFERSIZE, CopySize, Format, VaList);
        va_end(VaList);

        Write(LogString, Level);
    }
}

void QingLog::Write(const std::string & LogString, LogLevel Level)
{
    if (m_IsOkToWrite)
    {
        //src::severity_logger_mt<QingLog::LogLevel>& lg = QingLogger::get();
        BOOST_LOG_SEV(QingLogger::get(), Level) << LogString;
    }
}

bool QingLog::SetLogDirectory(const std::string &Directory)
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

bool QingLog::SetLogDirectoryAutoAppendProgramName(const std::string &Directory)
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
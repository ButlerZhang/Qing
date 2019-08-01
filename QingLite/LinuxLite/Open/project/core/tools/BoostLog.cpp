#include "BoostLog.h"
#include <boost/format.hpp>
#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <sys/stat.h>
#include <sys/types.h>
#include "../../../../LinuxTools.h"


BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(BoostLogger, boost::log::sources::severity_logger_mt<LogLevel>)
BoostLog g_Log;


// The formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, LogLevel lvl)
{
    switch (lvl)
    {
    case LL_TEMP:         strm << "TEMP ";                break;
    case LL_DEBUG:        strm << "DEBUG";                break;
    case LL_INFO:         strm << "INFO ";                break;
    case LL_ERROR:        strm << "ERROR";                break;
    default:              strm << std::to_string(lvl);    break;
    }

    return strm;
}



BoostLog::BoostLog()
{
    SetIsOkToWrite(true);
    SetLogDirectory(GetWorkDirectory());

    InitBaseSink(GetProgramName());
    InitTemporarySink(GetProgramName() + "_Temp");
}

BoostLog::~BoostLog()
{
    boost::log::core::get()->remove_all_sinks();
}
void BoostLog::Flush()
{
    boost::log::core::get()->flush();
}

void BoostLog::SetFilter(LogLevel Level)
{
    boost::log::core::get()->set_filter(boost::log::expressions::attr<LogLevel>("Severity") >= Level);
}

boost::shared_ptr<TextSink> BoostLog::CreateSink(const std::string & FileName)
{
    const size_t ONE_MB = 1024 * 1024;
    long ProcessID = (long)getpid();

    boost::shared_ptr<boost::log::sinks::text_file_backend> Backend = boost::make_shared<boost::log::sinks::text_file_backend>(
        boost::log::keywords::file_name = m_LogDirectory + FileName + "_%Y%m%d_%H%M%S_" + std::to_string(ProcessID) + ".log",
        boost::log::keywords::rotation_size = 10 * ONE_MB,
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        boost::log::keywords::min_free_space = 500 * ONE_MB);

    Backend->auto_flush(true);
    boost::shared_ptr<TextSink> NewSink(new TextSink(Backend));

    NewSink->set_formatter(
        boost::log::expressions::format("[%1%][%2%][%3%]: %4%")
        % boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
        % boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")
        % boost::log::expressions::attr<LogLevel>("Severity")
        % boost::log::expressions::smessage
    );

    return NewSink;
}

void BoostLog::InitBaseSink(const std::string &LogFileName)
{
    boost::shared_ptr<TextSink> BaseSink = CreateSink(LogFileName);
    BaseSink->set_filter(boost::log::expressions::attr<LogLevel>("Severity") >= LL_DEBUG);

    auto ConsoleSink = boost::log::add_console_log();
    ConsoleSink->set_filter(boost::log::expressions::attr<LogLevel>("Severity") >= LL_INFO);

    boost::log::core::get()->add_sink(BaseSink);
    boost::log::core::get()->add_sink(ConsoleSink);

    boost::log::add_common_attributes();
}

void BoostLog::InitTemporarySink(const std::string & LogFileName)
{
    boost::shared_ptr<TextSink> TempSink = CreateSink(LogFileName);
    TempSink->set_filter(boost::log::expressions::attr<LogLevel>("Severity") == LL_TEMP);

    boost::log::core::get()->add_sink(TempSink);
    boost::log::add_common_attributes();
}

void BoostLog::WriteLog(LogLevel Level, const std::string &LogString)
{
    if (m_IsOkToWrite)
    {
        BOOST_LOG_SEV(BoostLogger::get(), Level) << LogString;
        //boost::log::core::get()->flush();
    }
}

bool BoostLog::SetLogDirectory(const std::string &Directory)
{
    struct stat statbuff;
    if (lstat(Directory.c_str(), &statbuff) == 0 && S_ISDIR(statbuff.st_mode))
    {
        m_LogDirectory = Directory;
        if (m_LogDirectory[m_LogDirectory.size() - 1] != '/')
        {
            m_LogDirectory.append("/LogFile/");
        }

        return true;
    }

    return false;
}

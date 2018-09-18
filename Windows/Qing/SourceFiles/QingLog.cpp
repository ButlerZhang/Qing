#include "..\HeaderFiles\QingLog.h"

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

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(QingLogger, src::severity_logger_mt<QingLog::LogLevel>)
typedef sinks::synchronous_sink<sinks::text_file_backend> TextSink;

bool QingLog::m_IsOkToWrite = true;
std::string QingLog::m_LogDirectory;



void QingLog::DefaultInit()
{
    SetIsOkToWrite(true);
    SetLogDirectory("C:\\QingLog\\");

    InitBaseSink();
    InitAdditionalSink();
}

void QingLog::DefaultShutdown()
{
    logging::core::get()->remove_all_sinks();
}

void QingLog::SetFilter(LogLevel level)
{
    logging::core::get()->set_filter(expr::attr<QingLog::LogLevel>("Severity") >= level);
}

void QingLog::Write(const std::string & LogString, LogLevel level)
{
    if (m_IsOkToWrite)
    {
        //src::severity_logger_mt<QingLog::LogLevel>& lg = QingLogger::get();
        BOOST_LOG_SEV(QingLogger::get(), level) << LogString;
    }
}

auto QingLog::CreateSink(const std::string & FileName)
{
    const size_t ONE_MB = 1024 * 1024;

    boost::shared_ptr<sinks::text_file_backend> backend = boost::make_shared<sinks::text_file_backend>(
        keywords::file_name = m_LogDirectory + FileName + "_%Y-%m-%d_%H-%M-%S.log",
        keywords::rotation_size = 100 * ONE_MB,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::min_free_space = 500 * ONE_MB);

    backend->auto_flush(true);
    boost::shared_ptr<TextSink> base_sink(new TextSink(backend));

    base_sink->set_formatter(
        expr::format("[%1%][%2%][%3%]: %4%")
        % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
        % expr::attr<QingLog::LogLevel>("Severity")
        % expr::attr<attrs::current_thread_id::value_type>("ThreadID")
        % expr::smessage
    );

    return base_sink;
}

void QingLog::InitBaseSink(const std::string &LogFileName)
{
    boost::shared_ptr<TextSink> base_sink = CreateSink(LogFileName);
    base_sink->set_filter(expr::attr<QingLog::LogLevel>("Severity") >= QingLog::LL_DEBUG);

    logging::core::get()->add_sink(base_sink);
    logging::add_common_attributes();
}

void QingLog::InitAdditionalSink(const std::string & LogFileName)
{
    boost::shared_ptr<TextSink> additional_sink = CreateSink(LogFileName);
    additional_sink->set_filter(expr::attr<QingLog::LogLevel>("Severity") == QingLog::LL_UI);

    logging::core::get()->add_sink(additional_sink);
    logging::add_common_attributes();
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

QING_NAMESPACE_END
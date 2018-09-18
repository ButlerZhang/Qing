#include "..\HeaderFiles\QingLog.h"

#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

QING_NAMESPACE_BEGIN

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

typedef sinks::synchronous_sink<sinks::text_file_backend> TextSink;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(QingLogger, src::severity_logger_mt<QingLog::LogLevel>)



QingLog::QingLog()
{
    m_IsOkToWrite = true;
    m_LogDirectory = "C:\\QingLog\\";

    InitBaseSink();
    InitAdditionalSink();
}

QingLog::~QingLog()
{
    logging::core::get()->remove_all_sinks();
}

auto QingLog::GetSinkBackend(const std::string &FileName) const
{
    const size_t ONE_MB = 1024 * 1024;
    boost::shared_ptr<sinks::text_file_backend> backend = boost::make_shared<sinks::text_file_backend>(
        keywords::file_name = m_LogDirectory + FileName + "_%Y-%m-%d_%H-%M-%S.log",
        keywords::rotation_size = 100 * ONE_MB,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::min_free_space = 500 * ONE_MB);
    backend->auto_flush(true);
    return backend;
}

void QingLog::InitBaseSink(const std::string &LogFileName) const
{
    boost::shared_ptr<sinks::text_file_backend> backend = GetSinkBackend(LogFileName);
    backend->auto_flush(true);

    boost::shared_ptr<TextSink> base_sink(new TextSink(backend));

    base_sink->set_formatter(
        expr::format("[%1%][%2%][%3%]: %4%")
        % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
        % expr::attr<QingLog::LogLevel>("Severity")
        % expr::attr<attrs::current_thread_id::value_type>("ThreadID")
        % expr::smessage
    );

    base_sink->set_filter(expr::attr<QingLog::LogLevel>("Severity") >= QingLog::LL_DEBUG);
    logging::core::get()->add_sink(base_sink);

    logging::add_common_attributes();
}

void QingLog::InitAdditionalSink(const std::string & LogFileName) const
{
    boost::shared_ptr<sinks::text_file_backend> backend = GetSinkBackend(LogFileName);
    backend->auto_flush(true);
    boost::shared_ptr<TextSink> additional_sink(new TextSink(backend));

    additional_sink->set_formatter(
        expr::format("[%1%][%2%][%3%]: %4%")
        % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
        % expr::attr<QingLog::LogLevel>("Severity")
        % expr::attr<attrs::current_thread_id::value_type>("ThreadID")
        % expr::smessage
    );

    additional_sink->set_filter(expr::attr<QingLog::LogLevel>("Severity") == QingLog::LL_REPORT);
    logging::core::get()->add_sink(additional_sink);

    logging::add_common_attributes();
}

QingLog & QingLog::GetInstance()
{
    static QingLog m_Instance;
    return m_Instance;
}

void QingLog::UnitTest()
{
    for (int i = 0; i < 100000; i++)
    {
        QingLog::GetInstance().Write("A debug severity message", LL_DEBUG);
        QingLog::GetInstance().Write("An informational severity message", LL_INFO);
        QingLog::GetInstance().Write("An error severity message", LL_ERROR);
        QingLog::GetInstance().Write("A fatal severity message", LL_FATAL);
        QingLog::GetInstance().Write("A report severity message", LL_REPORT);
    }
}

void QingLog::Write(const std::string & LogString, LogLevel level) const
{
    if (m_IsOkToWrite)
    {
        //src::severity_logger_mt<QingLog::LogLevel>& lg = QingLogger::get();
        BOOST_LOG_SEV(QingLogger::get(), level) << LogString;
    }
}

QING_NAMESPACE_END
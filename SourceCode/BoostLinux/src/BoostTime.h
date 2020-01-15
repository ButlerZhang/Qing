#pragma once
#include <boost/date_time/posix_time/posix_time.hpp>



class BoostTime
{
public:

    BoostTime()
    {
        Restart();
    }

    BoostTime(const std::string &CurrentISOTime)
    {
        m_CurrentTime = boost::posix_time::from_iso_extended_string(CurrentISOTime);
    }

    ~BoostTime() {}

    void Restart()
    {
        m_CurrentTime = boost::posix_time::microsec_clock::local_time();
    }

    int64_t ElapsedMicroseconds() const
    {
        return (boost::posix_time::microsec_clock::local_time() - m_CurrentTime).total_microseconds();
    }

    int64_t ElapsedMilliseconds() const
    {
        return (boost::posix_time::microsec_clock::local_time() - m_CurrentTime).total_milliseconds();
    }

    int64_t ElapsedSeconds() const
    {
        return (boost::posix_time::microsec_clock::local_time() - m_CurrentTime).total_seconds();
    }

    std::string ElapsedString() const
    {
        //format: hh:mm:ss.ffffff
        return boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::local_time() - m_CurrentTime);
    }

    void SetCurrentISOTime(const std::string &ISOTime)          //format: 2019-10-17T12:00:00.123
    {
        m_CurrentTime = boost::posix_time::from_iso_extended_string(ISOTime);
    }

    void SetCurrentStringTime(const std::string &StringTime)    //format: 2019-10-17 12:00:00.123
    {
        m_CurrentTime = boost::posix_time::time_from_string(StringTime);
    }

    bool IsInRange(const std::string &StartTime, const std::string &StopTime) const;

    static std::string GetLocalTime()                           //format: 2019-10-17T12:00:00.123
    {
        return boost::posix_time::to_iso_extended_string(boost::posix_time::second_clock::local_time());
    }

    static bool SetLocalTime(const std::string &NewLocalTime);  //format: 2019-10-17T12:00:00.123

private:

    boost::posix_time::ptime m_CurrentTime;
};

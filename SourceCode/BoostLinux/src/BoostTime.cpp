#include "BoostTime.h"



bool BoostTime::IsInRange(const std::string &StartTime, const std::string &StopTime) const
{
    const boost::posix_time::ptime &Start = boost::posix_time::time_from_string(StartTime);
    if ((m_CurrentTime - Start).total_seconds() < 0)
    {
        return false;
    }

    const boost::posix_time::ptime &Stop = boost::posix_time::time_from_string(StopTime);
    if ((Stop - m_CurrentTime).total_seconds() < 0)
    {
        return false;
    }

    return true;
}

bool BoostTime::SetLocalTime(const std::string &NewLocalTime)
{
    if (NewLocalTime.empty())
    {
        return false;
    }

    boost::posix_time::ptime PTime = boost::posix_time::from_iso_extended_string(NewLocalTime);
    struct tm TMTime = boost::posix_time::to_tm(PTime);

    time_t LocalTime = mktime(&TMTime);
    if (LocalTime <= 0)
    {
        return false;
    }

#ifdef _WIN32

    return false;

#else

    struct timeval TimeVal;
    TimeVal.tv_sec = LocalTime;
    TimeVal.tv_usec = 0;
    return settimeofday(&TimeVal, NULL) == 0;

#endif
}

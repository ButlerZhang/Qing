#include "TimeInterval.h"
#include <time.h>
#include <sstream>
#include <iomanip>



TimeInterval::TimeInterval(void)
{
}

TimeInterval::~TimeInterval(void)
{
}

std::string TimeInterval::SecondToTimeString(int64 SecondTime)
{
    auto tm = *std::localtime(&SecondTime);

    std::ostringstream oss;

    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

int64 TimeInterval::TimeStringToSecond(const std::string & TimeString)
{
    std::tm t = { 0 };
    std::istringstream iss(TimeString);

    iss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");

    return iss.fail() ? 0 : std::mktime(&t);
}

int64 TimeInterval::SecondDistances(const Active::DateTime &NewDateTime, const Active::DateTime &OldDateTime)
{
    return SecondDistances(NewDateTime.GetYear(),
                           NewDateTime.GetMonth(),
                           NewDateTime.GetDay(),
                           NewDateTime.GetHour(),
                           NewDateTime.GetMinute(),
                           NewDateTime.GetSecond(),
                           OldDateTime.GetYear(),
                           OldDateTime.GetMonth(),
                           OldDateTime.GetDay(),
                           OldDateTime.GetHour(),
                           OldDateTime.GetMinute(),
                           OldDateTime.GetSecond());
}

int64 TimeInterval::SecondDistances(int newYear, int newMonth, int newDay, int newHour, int newMinth, int newSecond,
                                    int oldYear, int oldMonth, int oldday, int oldHour, int oldMinth, int oldSecond)
{
    double diff = Distances(newYear, newMonth, newDay, newHour, newMinth, newSecond,
                            oldYear, oldMonth, oldday, oldHour, oldMinth, oldSecond);

    return static_cast<int64>(diff);
}

double TimeInterval::Distances(int newYear, int newMonth, int newDay, int newHour, int newMinth, int newSecond,
                               int oldYear, int oldMonth, int oldday, int oldHour, int oldMinth, int oldSecond)
{
    if (newYear <= 1900 || oldYear <= 1900)
        return -1;

    time_t NewTime = ConvertToTimeT(newYear, newMonth, newDay, newHour, newMinth, newSecond);
    time_t OldTime = ConvertToTimeT(oldYear, oldMonth, oldday, oldHour, oldMinth, oldSecond);

    double diff = difftime(NewTime, OldTime);

    return diff;
}

time_t TimeInterval::ConvertToTimeT(int Year, int Month, int Day, int Hour, int Minth, int Second)
{
    struct tm tmTemp = { 0 };

    tmTemp.tm_year = Year - 1900;
    tmTemp.tm_mon = Month - 1;
    tmTemp.tm_mday = Day;
    tmTemp.tm_hour = Hour;
    tmTemp.tm_min = Minth;
    tmTemp.tm_sec = Second;

    time_t timeResult = mktime(&tmTemp);

    return timeResult;
}

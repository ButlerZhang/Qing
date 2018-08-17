#pragma once

#include "../../LibraryLite/Reference/Active/Include/DateTime.h"



class TimeInterval
{
public:

    TimeInterval(void);
    ~TimeInterval(void);

    static std::string SecondToTimeString(int64 SecondTime);
    static int64 TimeStringToSecond(const std::string &TimeString);

    static int64 SecondDistances(const Active::DateTime &NewDateTime, const Active::DateTime &OldDateTime);

    static int64 SecondDistances(int newYear,int newMonth,int newDay,int newHour,int newMinth,int newSecond,
                                 int oldYear,int oldMonth,int oldday,int oldHour,int oldMinth, int oldSecond);

private:

    static double Distances(int newYear,int newMonth,int newDay,int newHour,int newMinth,int newSecond,
                            int oldYear,int oldMonth,int oldday,int oldHour,int oldMinth, int oldSecond);

    static time_t ConvertToTimeT(int Year, int Month, int Day, int Hour, int Minth, int Second);
};

#pragma once
#include <Windows.h>



unsigned long TestTimeGetTime();



class ElapsedTime
{
public:

    ElapsedTime() { QueryPerformanceFrequency(&m_liPerfFreq); Start(); }
    void Start()  { QueryPerformanceCounter(&m_liPerfStart); }

    __int64 Now() const //Returns # of ms since Start was called
    {
        LARGE_INTEGER liPerfNow;
        QueryPerformanceCounter(&liPerfNow);
        return(((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart);
    }

private:

    LARGE_INTEGER m_liPerfFreq;     //Counts per second
    LARGE_INTEGER m_liPerfStart;    //Starting count
};

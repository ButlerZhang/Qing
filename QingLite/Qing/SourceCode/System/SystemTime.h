#pragma once
#include "../QingBase.h"
#include <Windows.h>

QING_NAMESPACE_BEGIN


unsigned long QING_DLL TestTimeGetTime();



class QING_DLL ElapsedTime
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

QING_NAMESPACE_END
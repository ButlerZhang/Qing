#include "SystemTime.h"

QING_NAMESPACE_BEGIN



__int64 TimeGetTime()
{
    //MSDN:
    //Note that the value returned by the timeGetTime function is a DWORD value.
    //The return value wraps around to 0 every 2 ^ 32 milliseconds, which is 
    //about 49.71 days.This can cause problems in code that directly uses the 
    //timeGetTime return value in computations, particularly where the value 
    //is used to control code execution.You should always use the difference 
    //between two timeGetTime return values in computations.

    static int WrapCount = 0;
    static unsigned long LastTimeStamp = 0;

    //lock

    const unsigned long CurrentTime = timeGetTime();
    if (CurrentTime < LastTimeStamp)
    {
        WrapCount += 1;
    }

    LastTimeStamp = CurrentTime;
    const __int64 FinalTimeStamp = (__int64(WrapCount) << 32) | CurrentTime;

    //unlock

    return FinalTimeStamp;
}



void GetTickCount64_Example()
{
    ULONGLONG StartTime = GetTickCount64();

    //TODO

    ULONGLONG StopTime = GetTickCount64();

    //通过GetTickCount64计算流逝时间需要一个前提:
    //即执行的代码不会被中断
    ULONGLONG ElapsedTime = StopTime - StartTime;
}

void ReadTimeStampCounter_Example()
{
    //跟GetTickCount64_Example存在问题，但是
    //ReadTimeStampCounter比GetTickCount64更精确

    ULONGLONG StartTime = ReadTimeStampCounter();
    //TODO
    ULONGLONG StopTime = ReadTimeStampCounter();
    ULONGLONG ElapsedTime = StopTime - StartTime;
}



__int64 FileTimeToQuadWord(PFILETIME pft)
{
    return (Int64ShllMod32(pft->dwHighDateTime, 32) | pft->dwLowDateTime);
}

void PerformLongOperation_Example()
{
    FILETIME ftDummy;

    //Get starting times.
    FILETIME ftKernelTimeStart, ftUserTimeStart;
    GetThreadTimes(GetCurrentThread(), &ftDummy, &ftDummy, &ftKernelTimeStart, &ftUserTimeStart);

    //Perform complex algorithm here.

    //Get ending times.
    FILETIME ftKernelTimeStop, ftUserTimeStop;
    GetThreadTimes(GetCurrentThread(), &ftDummy, &ftDummy, &ftKernelTimeStop, &ftUserTimeStop);

    //Get the elapsed kernel and user times by converting the start
    //and end times form FILETIMEs to quad words, and then subtract
    //the start times form the end times.
    __int64 qwKernelTimeElapsed = FileTimeToQuadWord(&ftKernelTimeStop) - FileTimeToQuadWord(&ftKernelTimeStart);
    __int64 qwUserTimeElapsed = FileTimeToQuadWord(&ftUserTimeStop) - FileTimeToQuadWord(&ftUserTimeStart);

    //Get total time duration by adding the kernel and user times.
    __int64 qwTotalTimeElapsed = qwKernelTimeElapsed + qwUserTimeElapsed;
}



void ElapsedTime_Example()
{
    ElapsedTime stopWatch;
    //TODO
    __int64 qwElapsedTime = stopWatch.Now();
}

QING_NAMESPACE_END

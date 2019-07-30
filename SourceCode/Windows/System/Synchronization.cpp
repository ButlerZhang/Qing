#include "Synchronization.h"



SlimRWLock::SlimRWLock()
{
    ::InitializeSRWLock(&m_SRWLock);
}

SlimRWLock::~SlimRWLock()
{
    //System auto clean up.
}

bool SlimRWLock::Lock() const
{
    ::AcquireSRWLockExclusive(&m_SRWLock);
    return true;
}

bool SlimRWLock::UnLock() const
{
    ::ReleaseSRWLockExclusive(&m_SRWLock);
    return true;
}

bool SlimRWLock::LockShared() const
{
    ::AcquireSRWLockShared(&m_SRWLock);
    return true;
}

bool SlimRWLock::UnLockShared() const
{
    ::ReleaseSRWLockShared(&m_SRWLock);
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

CriticalSection::CriticalSection(unsigned long SpinCount)
{
    if (SpinCount == 0)
    {
        //Maybe throw STATUS_NO_MEMORY execption
        ::InitializeCriticalSection(&m_Section);
    }
    else
    {
        //Switch to wait state when rotation reached SpinCount
        ::InitializeCriticalSectionAndSpinCount(&m_Section, SpinCount);
        //::SetCriticalSectionSpinCount(&m_Section, SpinCount); //0~0x00FFFFFF
    }
}

CriticalSection::~CriticalSection()
{
    ::DeleteCriticalSection(&m_Section);
}

bool CriticalSection::Lock() const
{
    try
    {
        //Maybe throw EXCEPTION_INVALID_HANDLE execption
        ::EnterCriticalSection(&m_Section);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool CriticalSection::UnLock() const
{
    ::LeaveCriticalSection(&m_Section);
    return true;
}

bool CriticalSection::TryLock() const
{
    return (::TryEnterCriticalSection(&m_Section) == TRUE);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

Event::Event(bool IsManualReset, bool InitialState, const TCHAR *Name, PSECURITY_ATTRIBUTES psa)
{
    m_hEvent = ::CreateEvent(psa, IsManualReset, InitialState, Name);
}

Event::~Event()
{
    CloseHandle(m_hEvent);
    m_hEvent = INVALID_HANDLE_VALUE;
}

bool Event::SetEvent()
{
    if (m_hEvent != INVALID_HANDLE_VALUE)
    {
        //Set trigger state
        return ::SetEvent(m_hEvent) == TRUE;
    }

    return false;
}

bool Event::ResetEvent()
{
    if (m_hEvent != INVALID_HANDLE_VALUE)
    {
        //Set untrigger state
        return ::ResetEvent(m_hEvent) == TRUE;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

Semaphore::Semaphore(long InitialCount, long MaxCount, const TCHAR * Name, PSECURITY_ATTRIBUTES psa)
{
    if (MaxCount <= 0)
    {
        MaxCount = 1;
    }

    if (InitialCount <= 0)
    {
        InitialCount = 1;
    }

    if (InitialCount > MaxCount)
    {
        InitialCount = MaxCount;
    }

    m_hSemaphore = ::CreateSemaphore(psa, InitialCount, MaxCount, Name);
}

Semaphore::~Semaphore()
{
    CloseHandle(m_hSemaphore);
    m_hSemaphore = INVALID_HANDLE_VALUE;
}

bool Semaphore::Lock() const
{
    DWORD Ret = ::WaitForSingleObject(m_hSemaphore, INFINITE);
    return (Ret == WAIT_OBJECT_0 || Ret == WAIT_ABANDONED);
}

bool Semaphore::UnLock() const
{
    return UnLock(1, NULL);
}

bool Semaphore::UnLock(long ReleaseCount, long * PrevReleaseCount) const
{
    return ::ReleaseSemaphore(m_hSemaphore, ReleaseCount, PrevReleaseCount) == TRUE;
}

#include "Synchronization.h"

QING_NAMESPACE_BEGIN



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

QING_NAMESPACE_END
#pragma once
#include "..\QingBase.h"
#include <Windows.h>

QING_NAMESPACE_BEGIN



class QING_DLL SlimRWLock
{
public:

    SlimRWLock();
    virtual ~SlimRWLock();

    virtual bool Lock() const;
    virtual bool UnLock() const;

    bool LockShared() const;
    bool UnLockShared() const;

private:

    mutable SRWLOCK m_SRWLock;
};



class QING_DLL CriticalSection
{
public:

    CriticalSection(unsigned long SpinCount = 0);
    virtual ~CriticalSection();

    virtual bool Lock() const;
    virtual bool UnLock() const;

    bool TryLock() const;

private:

    mutable CRITICAL_SECTION m_Section;
};



class QING_DLL Event
{
public:

    Event(bool IsManualReset, bool InitialState, const TCHAR *Name = NULL, PSECURITY_ATTRIBUTES psa = NULL);
    ~Event();

    bool SetEvent();
    bool ResetEvent();

private:

    HANDLE m_hEvent;
};



class QING_DLL Semaphore
{
public:

    Semaphore(long InitialCount, long MaxCount, const TCHAR *Name = NULL, PSECURITY_ATTRIBUTES psa = NULL);
    ~Semaphore();

    virtual bool Lock() const;
    virtual bool UnLock() const;
    bool UnLock(long ReleaseCount, long *PrevReleaseCount = 0) const;

private:

    HANDLE m_hSemaphore;
};

QING_NAMESPACE_END
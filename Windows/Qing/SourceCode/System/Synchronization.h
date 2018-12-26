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

QING_NAMESPACE_END
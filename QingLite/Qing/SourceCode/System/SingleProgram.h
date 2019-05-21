#pragma once
#include "../QingBase.h"
#include <string>
#include <Windows.h>

QING_NAMESPACE_BEGIN



class QING_DLL SingleProgram
{
public:

    SingleProgram(void) : m_Mutex(NULL)
    {
    }

    ~SingleProgram(void)
    {
        MandatoryDestroyInstance();
    }

    bool CreateSingleInstance(const std::wstring& ProgramName)
    {
        m_Mutex = ::CreateMutex(NULL,TRUE,ProgramName.c_str());
        DWORD LastError = GetLastError();

        if(m_Mutex != NULL)
        {
            ::ReleaseMutex(m_Mutex);
        }

        if(LastError == ERROR_ALREADY_EXISTS)
        {
            m_Mutex = NULL;
            return false;
        }

        return true;
    }

    bool MandatoryDestroyInstance()
    {
        if(m_Mutex != NULL)
        {
            ::CloseHandle(m_Mutex);
            m_Mutex = NULL;
        }

        return true;
    }

private:

    SingleProgram(SingleProgram&);
    SingleProgram& operator=(SingleProgram&);

    HANDLE m_Mutex;
};

QING_NAMESPACE_END
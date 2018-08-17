#pragma once
#include "QingBase.h"
#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <stdio.h>
#include <crtdbg.h>
#include <string>

#pragma comment ( lib, "dbghelp.lib" )

QING_NAMESPACE_BEGIN



const std::string GetProgramName()
{
    char FullPathCharArray[MAX_PATH];
    memset(FullPathCharArray, 0, sizeof(FullPathCharArray));
    ::GetModuleFileNameA(NULL, FullPathCharArray, MAX_PATH);

    std::string FullPathString(FullPathCharArray);
    if (!FullPathString.empty())
    {
        std::size_t StartIndex = FullPathString.find_last_of('\\') + 1;
        if (StartIndex != std::string::npos)
        {
            std::size_t CharCount = FullPathString.size() - StartIndex - 4; //sizeof(.exe) == 4
            std::string ProgramName = (FullPathString.substr(StartIndex, CharCount) + ".dmp");
            return ProgramName;
        }
    }

    return "LTI.dmp";
}



static LONG WINAPI ProgramUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionPointers)
{
    HANDLE hFile = CreateFileA( GetProgramName().c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL );

    if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION Mdei;

        Mdei.ThreadId           = GetCurrentThreadId();
        Mdei.ExceptionPointers  = ExceptionPointers;
        Mdei.ClientPointers     = FALSE;

        MINIDUMP_TYPE Mdt       = (MINIDUMP_TYPE)(MiniDumpWithFullMemory |      //包含进程地址空间中所有可读页面的内容
            MiniDumpWithFullMemoryInfo |                                        //包含进程虚拟内存布局的完整信息
            MiniDumpWithHandleData |                                            //包含故障时刻进程故障表里面的所有句柄
            MiniDumpWithThreadInfo |                                            //包含进程中线程的附加信息
            MiniDumpWithUnloadedModules );                                      //包含所有模块信息

        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hFile, Mdt, &Mdei, NULL, NULL);
        CloseHandle(hFile);
    }

    MessageBox(NULL, _T("Program is encountering a fatal error and going to shutdown"), _T("LTI"), MB_OK | MB_ICONERROR);
    return EXCEPTION_EXECUTE_HANDLER;
}



void QING_DLL SetProgramDMP()
{
    //EnableProgramDMP is false in the release version.
    bool EnableProgramDMP = false;

    if (EnableProgramDMP)
    {
        SetUnhandledExceptionFilter(ProgramUnhandledExceptionFilter);
    }
}

QING_NAMESPACE_END
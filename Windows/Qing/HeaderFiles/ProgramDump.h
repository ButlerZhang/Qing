#pragma once
#include "QingBase.h"
#include "Utility.h"
#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <stdio.h>
#include <crtdbg.h>
#include <string>

#pragma comment ( lib, "dbghelp.lib" )

QING_NAMESPACE_BEGIN



static LONG WINAPI ProgramUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionPointers)
{
    HANDLE hFile = CreateFileA((GetProgramName()+".dmp").c_str(),
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

        MINIDUMP_TYPE Mdt       = (MINIDUMP_TYPE)(MiniDumpWithFullMemory |      //�������̵�ַ�ռ������пɶ�ҳ�������
            MiniDumpWithFullMemoryInfo |                                        //�������������ڴ沼�ֵ�������Ϣ
            MiniDumpWithHandleData |                                            //��������ʱ�̽��̹��ϱ���������о��
            MiniDumpWithThreadInfo |                                            //�����������̵߳ĸ�����Ϣ
            MiniDumpWithUnloadedModules );                                      //��������ģ����Ϣ

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
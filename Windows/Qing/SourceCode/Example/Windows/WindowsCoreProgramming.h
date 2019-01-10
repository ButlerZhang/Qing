#pragma once
#include <tchar.h>
#include <strsafe.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>



void StartRestrictedProcess()
{
    //Check if we are not already associated with a job.
    //If this is the case, there is no way to switch to another job.
    BOOL bInJob = FALSE;
    IsProcessInJob(GetCurrentProcess(), NULL, &bInJob);
    if (bInJob)
    {
        MessageBox(NULL, TEXT("Process already in a job."), TEXT(""), MB_ICONINFORMATION | MB_OK);
        return;
    }

    //Create a job kernel object
    HANDLE hjob = CreateJobObject(NULL, TEXT("Wintellect_RestrictedProessJob"));

    //Place some restrictions on processes in the job

    //First, set some basic restrictions.
    JOBOBJECT_BASIC_LIMIT_INFORMATION jobli = { 0 };

    //The process always runs in the idle priority class.
    jobli.PriorityClass = IDLE_PRIORITY_CLASS;

    //The job cannot use more than 1 second of CPU time.
    jobli.PerJobUserTimeLimit.QuadPart = 10000; //1 sec in 100-ns intervals

    //These are the only 2 restrictions I want placed on the job(process).
    jobli.LimitFlags = JOB_OBJECT_LIMIT_PRIORITY_CLASS | JOB_OBJECT_LIMIT_JOB_TIME;
    SetInformationJobObject(hjob, JobObjectBasicLimitInformation, &jobli, sizeof(jobli));

    //Second, set some UI restrictions
    JOBOBJECT_BASIC_UI_RESTRICTIONS jobuir;
    jobuir.UIRestrictionsClass = JOB_OBJECT_UILIMIT_NONE;   //A fancy zero

    //The process can not log off the system.
    jobuir.UIRestrictionsClass |= JOB_OBJECT_UILIMIT_EXITWINDOWS;

    //The process can not access USER objects(such as other windows) in the system
    jobuir.UIRestrictionsClass |= JOB_OBJECT_UILIMIT_HANDLES;
    SetInformationJobObject(hjob, JobObjectBasicUIRestrictions, &jobuir, sizeof(jobuir));

    //Spawn the process that is to be in the job.
    //Note: You must first spawn the process and then place the process in the job.
    //      This means that the process's thread must be initially suspended so that
    //      it can not execute any code outside of the job's restrictions.
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    TCHAR szCmdLine[8];
    _tcscpy_s(szCmdLine, _countof(szCmdLine), TEXT("CMD"));
    BOOL bResult = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE,
        CREATE_SUSPENDED | CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

    //Place the process in the job
    //Note: If this process spawns any children, the children are automatically
    //      part of the same job.
    AssignProcessToJobObject(hjob, pi.hProcess);

    //Now we can allow the child process' thread to execute code.
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);

    //Wait for the process to terminate or
    //for all the job's allotted CPU time to be used.
    HANDLE h[2];
    h[0] = pi.hProcess;
    h[1] = hjob;
    DWORD dw = WaitForMultipleObjects(2, h, FALSE, INFINITE);
    switch (dw - WAIT_OBJECT_0)
    {
    case 0:
        //The process has terminated.
        break;
    case 1:
        //All of the job's allotted CPU time was used.
        break;
    }

    FILETIME CreationTime;
    FILETIME ExitTime;
    FILETIME KernelTime;
    FILETIME UserTime;
    TCHAR szInfo[MAX_PATH];
    GetProcessTimes(pi.hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime);

    StringCchPrintf(szInfo, _countof(szInfo), TEXT("Kernel = %u | User = %u\n"),
        KernelTime.dwLowDateTime / 10000, UserTime.dwLowDateTime / 10000);

    MessageBox(GetActiveWindow(), szInfo, TEXT("Restricted Process times"), MB_ICONINFORMATION | MB_OK);

    //Clean up properly
    CloseHandle(pi.hProcess);
    CloseHandle(hjob);
}



void SuspendProcess(DWORD dwProcessID, BOOL fSuspend)
{
    //Get the list of threads in the system.
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessID);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        //Walk the list of threas.
        THREADENTRY32 te = { sizeof(te) };
        for (BOOL fOK = Thread32First(hSnapshot, &te); fOK; fOK = Thread32Next(hSnapshot, &te))
        {
            //Is this tread in the desired process?
            if (te.th32OwnerProcessID == dwProcessID)
            {
                //Attempt to convert the thread ID into a handle
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (hThread != NULL)
                {
                    //Suspend or resume the thread
                    fSuspend ? SuspendThread(hThread) : ResumeThread(hThread);
                }

                CloseHandle(hThread);
            }
        }
        CloseHandle(hSnapshot);
    }
}



void AlignExample()
{
    struct CUSTINFO_1 {
        DWORD   dwCustomerID;     //mostly read-only
        int     nBalanceDue;      //read-write
        wchar_t szName[100];      //mostly read-only
        FILETIME ftLastOrderDate; //read-write
    };

    struct CUSTINFO_2 {
        DWORD   dwCustomerID;     //mostly read-only
        wchar_t szName[100];      //mostly read-only

        int     nBalanceDue;      //read-write
        FILETIME ftLastOrderDate; //read-write
    };

    //调整对齐后:
#define CACHE_ALIGN 64

    //Force each structure to be in a different cache line
    struct __declspec(align(CACHE_ALIGN)) CUSTINFO_3 {
        DWORD   dwCustomerID;     //mostly read-only
        wchar_t szName[100];      //mostly read-only

                                  //Force the following members to be in a difference cache line
        int     nBalanceDue;      //read-write
        FILETIME ftLastOrderDate; //read-write
    };

    std::cout << "char = " << sizeof(char) << std::endl;
    std::cout << "wchar_t = " << sizeof(wchar_t) << std::endl;
    std::cout << "DWORD = " << sizeof(DWORD) << std::endl;
    std::cout << "int = " << sizeof(int) << std::endl;
    std::cout << "FILETIME = " << sizeof(FILETIME) << std::endl << std::endl;

    std::cout << "CUSTINFO_1 = " << sizeof(CUSTINFO_1) << std::endl;
    std::cout << "CUSTINFO_2 = " << sizeof(CUSTINFO_2) << std::endl;
    std::cout << "CUSTINFO_3 = " << sizeof(CUSTINFO_3) << std::endl << std::endl;
}



void SetTimer1()
{
    //First signaling
    SYSTEMTIME st;
    st.wYear = 2018;
    st.wMonth = 1;
    st.wDayOfWeek = 0;
    st.wDay = 1;
    st.wHour = 1;
    st.wMinute = 0;
    st.wSecond = 0;
    st.wMilliseconds = 0;

    FILETIME ftLocal;
    SystemTimeToFileTime(&st, &ftLocal);

    //Convert local time to UTC time
    FILETIME ftUTC;
    LocalFileTimeToFileTime(&ftLocal, &ftUTC);

    //Convert FILETIME to LARGE_INTEGER because of different alignment
    LARGE_INTEGER liUTC;
    liUTC.LowPart = ftUTC.dwLowDateTime;
    liUTC.HighPart = ftUTC.dwHighDateTime;

    //Create an auto-reset timer
    HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

    //Set timer
    SetWaitableTimer(hTimer, &liUTC, 6 * 60 * 60 * 1000, NULL, NULL, FALSE);
}

void SetTimer2()
{
    //Timer unit is 100 nanoseconds.
    const int nTimerUnitsPerSecond = 10000000;

    //Set the timer to go off 5 seoncds after calling SetWaitableTimer.
    //Negate the time so that SetWaitableTimer knows we want 
    //relative time instead of absolute time.
    LARGE_INTEGER li;
    li.QuadPart = -(5 * nTimerUnitsPerSecond);

    //Set timer
    HANDLE hTimer = ::CreateWaitableTimer(NULL, FALSE, NULL);
    SetWaitableTimer(hTimer, &li, 6 * 60 * 60 * 1000, NULL, NULL, FALSE);
}



HANDLE CreateNewCompletionPort(DWORD dwNumberOfConcurrentThreads)
{
    //1.创建完成端口时前三个参数固定
    //2.dwNumberOfConcurrentThreads表示同一时间最多能有多少条线程处于
    //可运行状态，传入0表示默认使用CPU数量
    return (CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0,
        dwNumberOfConcurrentThreads));
}

BOOL AssociateDeviceWithCompletionPort(
    HANDLE hCompletionPort, HANDLE hDevice, DWORD dwCompletionKey)
{
    HANDLE h = CreateIoCompletionPort(
        hDevice,            //设备句柄（文件、套接字、邮件槽、管道等）
        hCompletionPort,    //已经存在的完成端口（CreateNewCompletionPort创建）
        dwCompletionKey,    //完成键，传给处理函数的参数，操作系统不关心这个值
        0);                 //同一时间可运行线程数量，绑定时忽略
    return (h == hCompletionPort);
}

void PrcessGetQueuedCompletionStatusReturn()
{
    HANDLE hIOCP;
    DWORD dwNumBytes;
    ULONG_PTR CompletionKey;
    OVERLAPPED *pOverlapped;

    BOOL bOK = GetQueuedCompletionStatus(hIOCP,
        &dwNumBytes, &CompletionKey, &pOverlapped, 1000);
    DWORD dwError = GetLastError();

    if (bOK) {
        //Process a successfully completed IO request
    }
    else {
        if (pOverlapped != NULL) {
            //Process a failed completed IO request
            //dwError contains the reason for failure
        }
        else {
            if (dwError == WAIT_TIMEOUT) {
                //Time-out while waitting for completed IO entry
            }
            else {
                //Bad call to GetQueuedCompletionStatus
                //dwError contains the reason for the bad call
            }
        }
    }
}
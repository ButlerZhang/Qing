#pragma once
#include <tchar.h>
#include <strsafe.h>
#include <Windows.h>
#include <TlHelp32.h>



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

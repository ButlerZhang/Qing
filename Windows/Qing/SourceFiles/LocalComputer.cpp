#include "..\HeaderFiles\LocalComputer.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <IPHlpApi.h>
#include <algorithm>
#include <tlhelp32.h>
#include <ATLComTime.h>

#pragma comment(lib, "Iphlpapi.lib")

namespace Qing
{
    LocalComputer::LocalComputer(void)
    {
    }

    LocalComputer::~LocalComputer(void)
    {
    }

    void LocalComputer::SetTaskAndStartMenuVisible(int VisibleValue) const
    {
        HWND Task = FindWindow(L"Shell_TrayWnd", NULL);
        ShowWindow(Task, VisibleValue);

        HWND StartMenu = ::FindWindow(L"Button", NULL);
        if (StartMenu != NULL)
        {
            ::ShowWindow(StartMenu, VisibleValue);
        }
    }

    bool LocalComputer::GetMacAddress(std::string &MacAddress, const char* ClientIP) const
    {
        PIP_ADAPTER_INFO PipAdapterInfo = new IP_ADAPTER_INFO();
        unsigned long StSize = sizeof(IP_ADAPTER_INFO);
        int Rel = GetAdaptersInfo(PipAdapterInfo, &StSize);

        if (ERROR_BUFFER_OVERFLOW == Rel)
        {
            delete PipAdapterInfo;
            PipAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[StSize];
            Rel = GetAdaptersInfo(PipAdapterInfo, &StSize);
        }

        if (ERROR_SUCCESS == Rel)
        {
            char Adress[512];
            memset(Adress, 0, sizeof(Adress));

            std::string TempMacAddress;
            PIP_ADAPTER_INFO TempIPAdapterInfo = PipAdapterInfo;
            while (TempIPAdapterInfo)
            {
                IP_ADDR_STRING *pIpAddrString = &(TempIPAdapterInfo->IpAddressList);
                while (pIpAddrString != NULL)
                {
                    if (strcmp(pIpAddrString->IpAddress.String, ClientIP) == 0)
                    {
                        for (UINT i = 0; i < PipAdapterInfo->AddressLength; i++)
                        {
                            sprintf_s(Adress, (i == PipAdapterInfo->AddressLength - 1) ? "%02x\n" : "%02x-", PipAdapterInfo->Address[i]);
                            TempMacAddress += Adress;
                        }

                        std::transform(TempMacAddress.begin(), TempMacAddress.end(), TempMacAddress.begin(), ::toupper);
                        MacAddress = TempMacAddress;

                        break;
                    }

                    pIpAddrString = pIpAddrString->Next;
                }

                TempIPAdapterInfo = TempIPAdapterInfo->Next;
            }
        }

        if (PipAdapterInfo)
        {
            delete PipAdapterInfo;
        }

        return !MacAddress.empty();
    }

    bool LocalComputer::GetLocalIPAddress(std::vector<std::string> &IPVector) const
    {
        PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
        unsigned long stSize = sizeof(IP_ADAPTER_INFO);

        int RealSize = GetAdaptersInfo(pIpAdapterInfo, &stSize);
        if (ERROR_BUFFER_OVERFLOW == RealSize)
        {
            delete pIpAdapterInfo;
            pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
            RealSize = GetAdaptersInfo(pIpAdapterInfo, &stSize);
        }

        if (ERROR_SUCCESS == RealSize)
        {
            PIP_ADAPTER_INFO TempIPAdapterInfo = pIpAdapterInfo;
            while (TempIPAdapterInfo)
            {
                IP_ADDR_STRING *pIpAddrString = &(TempIPAdapterInfo->IpAddressList);
                while (pIpAddrString != NULL)
                {
                    IPVector.push_back(pIpAddrString->IpAddress.String);
                    pIpAddrString = pIpAddrString->Next;
                }

                TempIPAdapterInfo = TempIPAdapterInfo->Next;
            }
        }

        if (pIpAdapterInfo)
        {
            delete pIpAdapterInfo;
        }

        return !IPVector.empty();
    }

    bool LocalComputer::IsExistProcess(const std::wstring &ProgramName) const
    {
        TCHAR MyTarget[MAX_PATH];
        lstrcpyn(MyTarget, ProgramName.c_str(), sizeof(MyTarget) / 2);
        CharLowerBuff(MyTarget, MAX_PATH);

        bool IsExist = false;

        PROCESSENTRY32 MyProcess;
        HANDLE MyHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (MyHandle != INVALID_HANDLE_VALUE)
        {
            MyProcess.dwSize = sizeof(MyProcess);

            if (Process32First(MyHandle, &MyProcess))
            {
                do
                {
                    CharLowerBuff(MyProcess.szExeFile, MAX_PATH);
                    if (lstrcmp(MyTarget, MyProcess.szExeFile) == 0)
                    {
                        IsExist = true;
                        break;
                    }
                } while (Process32Next(MyHandle, &MyProcess));
            }

            CloseHandle(MyHandle);
        }

        return IsExist;
    }

    bool LocalComputer::KillProgram(const std::wstring & ProgramName) const
    {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (!Process32First(hSnapShot, &pe))
        {
            return false;
        }

        std::wstring LowerProgramName;
        LowerProgramName.resize(ProgramName.size());
        std::transform(ProgramName.begin(), ProgramName.end(), LowerProgramName.begin(), ::tolower);

        while (Process32Next(hSnapShot, &pe))
        {
            CString scTmp(pe.szExeFile);
            if (!(scTmp.MakeLower()).Compare(LowerProgramName.c_str()))
            {
                DWORD dwProcessID = pe.th32ProcessID;
                HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
                ::TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        }

        return true;
    }

    bool LocalComputer::StartProgram(const std::wstring & ProgramName) const
    {
        ShellExecute(NULL, L"open", ProgramName.c_str(), NULL, NULL, SW_SHOWNORMAL);
        int ErrorValue = GetLastError();
        if (ErrorValue != 0)
        {
            LPVOID lpMsgBuf;
            DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
            if (FormatMessage(dwFlags, NULL, ErrorValue, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL))
            {
                CString Info = (LPCTSTR)lpMsgBuf;
                return false;
            }
        }

        return true;
    }

    int LocalComputer::StartTouchScreenCalibration(HWND CurrentHWND) const
    {
        PVOID oldValue;
        Wow64DisableWow64FsRedirection(&oldValue);

        ShellExecuteA(CurrentHWND, "open", "C:\\Windows\\System32\\tabcal.exe", NULL, NULL, SW_SHOWNORMAL);
        int ShellExecuteError = GetLastError();

        BOOLEAN tmpBoolean = TRUE;
        Wow64EnableWow64FsRedirection(tmpBoolean);

        return ShellExecuteError;
    }

    std::string LocalComputer::TranslateDateTime(const std::string &SourceTime) const
    {
        if (!SourceTime.empty())
        {
            COleDateTime dateTime;
            dateTime.ParseDateTime(CString(SourceTime.c_str()));

#ifdef  UNICODE
            std::wstring wStrTime = dateTime.Format(_T("%Y-%m-%d %H:%M:%S")).GetBuffer(0);
            return std::string();//TODO
                                 //return WStringToString(wStrTime);
#else
            std::string StrTime = dateTime.Format(_T("%Y-%m-%d %H:%M:%S")).GetBuffer(0);
            return StrTime;
#endif
        }

        return std::string();
    }

    BOOL LocalComputer::SetOSDateTimeFormat() const
    {
        BOOL ret = TRUE;

        //short date
        TCHAR shortDate[64] = { 0 };
        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, shortDate, sizeof(shortDate));
        if (_tcscmp(shortDate, _T("yyyy-MM-dd")) != 0)
        {
            ret = ret && SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, _T("yyyy-MM-dd"));
        }

        //long date
        TCHAR longDate[64] = { 0 };
        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLONGDATE, longDate, sizeof(longDate));
        if (_tcscmp(longDate, _T("MMMM d,yyyy")) != 0)
        {
            ret = ret && SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLONGDATE, _T("MMMM d,yyyy"));
        }

        //short time  LOCALE_SSHORTTIME
        TCHAR shortTime[64] = { 0 };
        GetLocaleInfo(LOCALE_USER_DEFAULT, 0x00000079, shortTime, sizeof(shortTime));
        if (_tcscmp(shortTime, _T("HH:mm")) != 0)
        {
            ret = ret && SetLocaleInfo(LOCALE_USER_DEFAULT, 0x00000079, _T("HH:mm"));
        }

        //long time
        TCHAR longTime[64] = { 0 };
        GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, longTime, sizeof(longTime));
        if (_tcscmp(longTime, _T("HH:mm:ss")) != 0)
        {
            ret = ret && SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, _T("HH:mm:ss"));
        }

        return  ret;
    }
}

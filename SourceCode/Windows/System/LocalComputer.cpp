#include "LocalComputer.h"
#include "../System/SystemShare.h"
#include "../../SourceCode/Common/Boost/BoostLog.h"

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <IPHlpApi.h>
#include <algorithm>
#include <tlhelp32.h>
#include <ATLComTime.h>
#include <WS2tcpip.h>
#include <ShlObj.h>



LocalComputer::LocalComputer(void)
{
}

LocalComputer::~LocalComputer(void)
{
}

bool LocalComputer::IsProgramExisted(const std::string &ProgramName) const
{
    const std::wstring &TempProgramName = StringToWString(ProgramName);
    return IsProgramExisted(TempProgramName);
}

bool LocalComputer::IsProgramExisted(const std::wstring & ProgramName) const
{
    bool IsExisted = false;

    HANDLE SnapshotHandler = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (SnapshotHandler == INVALID_HANDLE_VALUE)
    {
        g_Log.WriteError("IsProgramExisted, SnapshotHandler == INVALID_HANDLE_VALUE");
        return IsExisted;
    }

    PROCESSENTRY32 ProcessInfo;
    ProcessInfo.dwSize = sizeof(ProcessInfo);
    if (!Process32First(SnapshotHandler, &ProcessInfo))
    {
        g_Log.WriteError("IsProgramExisted, Process32First failed.");
        CloseHandle(SnapshotHandler);
        return IsExisted;
    }

    std::wstring UpperProgramName(ProgramName);
    std::transform(ProgramName.begin(), ProgramName.end(), UpperProgramName.begin(), ::tolower);

    do
    {
        CharLowerBuff(ProcessInfo.szExeFile, MAX_PATH);
        if (lstrcmp(UpperProgramName.c_str(), ProcessInfo.szExeFile) == 0)
        {
            IsExisted = true;
            break;
        }
    } while (Process32Next(SnapshotHandler, &ProcessInfo));

    CloseHandle(SnapshotHandler);
    return IsExisted;
}

bool LocalComputer::SetOSDateTimeFormat(/*YYYY-MM-DD HH:mm:ss*/) const
{
    BOOL result = TRUE;

    //short date
    TCHAR shortDate[64] = { 0 };
    GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, shortDate, sizeof(shortDate));
    if (_tcscmp(shortDate, _T("yyyy-MM-dd")) != 0)
    {
        result = result && SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, _T("yyyy-MM-dd"));
    }

    //long date
    TCHAR longDate[64] = { 0 };
    GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLONGDATE, longDate, sizeof(longDate));
    if (_tcscmp(longDate, _T("MMMM d,yyyy")) != 0)
    {
        result = result && SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLONGDATE, _T("MMMM d,yyyy"));
    }

    //short time  LOCALE_SSHORTTIME
    TCHAR shortTime[64] = { 0 };
    GetLocaleInfo(LOCALE_USER_DEFAULT, 0x00000079, shortTime, sizeof(shortTime));
    if (_tcscmp(shortTime, _T("HH:mm")) != 0)
    {
        result = result && SetLocaleInfo(LOCALE_USER_DEFAULT, 0x00000079, _T("HH:mm"));
    }

    //long time
    TCHAR longTime[64] = { 0 };
    GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, longTime, sizeof(longTime));
    if (_tcscmp(longTime, _T("HH:mm:ss")) != 0)
    {
        result = result && SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, _T("HH:mm:ss"));
    }

    return (result == TRUE);
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

bool LocalComputer::StartProgram(const std::string &ProgramName) const
{
    const std::wstring &TempProgramName = StringToWString(ProgramName);
    return StartProgram(TempProgramName);
}

bool LocalComputer::StartProgram(const std::wstring &ProgramName) const
{
    ShellExecute(NULL, L"open", ProgramName.c_str(), NULL, NULL, SW_SHOWNORMAL);
    DWORD ErrorValue = GetLastError();
    if (ErrorValue != 0)
    {
        const std::wstring &ErrorMessage = GetLastErrorString(ErrorValue);
        g_Log.WriteError(BoostFormat("Start program fail, program name = %s, error = %s.", ProgramName.c_str(), ErrorMessage.c_str()));
        return false;
    }

    return true;
}

bool LocalComputer::StartProgramEx(const std::wstring & ProgramName) const
{
    //Initialize the structure
    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };

    //Ask for privileges elevation
    sei.lpVerb = TEXT("runas");

    //sei.lpFile = TEXT("cmd.exe");
    sei.lpFile = ProgramName.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (!(ShellExecuteEx(&sei)))
    {
        DWORD dwStatus = GetLastError();
        //if (dwStatus == ERROR_CANCELLED)
        //{
        //    //User refused to allow privileges elevation.
        //}

        const std::wstring &ErrorMessage = GetLastErrorString(dwStatus);
        g_Log.WriteError(BoostFormat("Start program fail, program name = %s, error = %s.", ProgramName.c_str(), ErrorMessage.c_str()));
        return false;
    }

    return true;
}

bool LocalComputer::StartTouchScreenCalibration(HWND CurrentHWND) const
{
    PVOID oldValue;
    Wow64DisableWow64FsRedirection(&oldValue);

    bool result = StartProgram(L"C:\\Windows\\System32\\tabcal.exe");

    BOOLEAN tmpBoolean = TRUE;
    Wow64EnableWow64FsRedirection(tmpBoolean);

    return result;
}

bool LocalComputer::KillProgram(const std::string & ProgramName) const
{
    const std::wstring &TempProgramName = StringToWString(ProgramName);
    return KillProgram(TempProgramName);
}

bool LocalComputer::KillProgram(const std::wstring & ProgramName) const
{
    HANDLE SnapshotHandler = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (SnapshotHandler == INVALID_HANDLE_VALUE)
    {
        g_Log.WriteError("KillProgram, SnapshotHandler == INVALID_HANDLE_VALUE");
        return false;
    }

    PROCESSENTRY32 ProcessInfo;
    ProcessInfo.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(SnapshotHandler, &ProcessInfo))
    {
        g_Log.WriteError("KillProgram, Process32First failed.");
        return false;
    }

    std::wstring LowerProgramName(ProgramName);
    std::transform(ProgramName.begin(), ProgramName.end(), LowerProgramName.begin(), ::tolower);

    while (Process32Next(SnapshotHandler, &ProcessInfo))
    {
        CString ExeFileName(ProcessInfo.szExeFile);
        if (!(ExeFileName.MakeLower()).Compare(LowerProgramName.c_str()))
        {
            DWORD dwProcessID = ProcessInfo.th32ProcessID;
            HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
            ::TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);

            //TerminateProcess是异步函数，需要添加等待机制
            //WaitForSingleObject(hProcess, INFINITE);
        }
    }

    return true;
}

bool LocalComputer::GetIPAddress(std::vector<std::string> &IPVector) const
{
    PIP_ADAPTER_INFO pIPAdapterInfo = new IP_ADAPTER_INFO();
    unsigned long IPAdapterInfoSize = sizeof(IP_ADAPTER_INFO);

    int IPAdapterInfoRealSize = GetAdaptersInfo(pIPAdapterInfo, &IPAdapterInfoSize);
    if (ERROR_BUFFER_OVERFLOW == IPAdapterInfoRealSize)
    {
        delete pIPAdapterInfo;
        pIPAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[IPAdapterInfoSize];
        IPAdapterInfoRealSize = GetAdaptersInfo(pIPAdapterInfo, &IPAdapterInfoSize);
    }

    if (ERROR_SUCCESS == IPAdapterInfoRealSize)
    {
        while (pIPAdapterInfo != NULL)
        {
            IP_ADDR_STRING *pIpAddrString = &(pIPAdapterInfo->IpAddressList);
            while (pIpAddrString != NULL)
            {
                IPVector.push_back(pIpAddrString->IpAddress.String);
                pIpAddrString = pIpAddrString->Next;
            }

            pIPAdapterInfo = pIPAdapterInfo->Next;
        }
    }

    delete pIPAdapterInfo;
    return !IPVector.empty();
}

bool LocalComputer::GetMacAddress(std::string &MacAddress, const std::string &BaseIP) const
{
    PIP_ADAPTER_INFO pIPAdapterInfo = new IP_ADAPTER_INFO();
    unsigned long IPAdapterInfoSize = sizeof(IP_ADAPTER_INFO);

    int IPAdapterInfoRealSize = GetAdaptersInfo(pIPAdapterInfo, &IPAdapterInfoSize);
    if (ERROR_BUFFER_OVERFLOW == IPAdapterInfoRealSize)
    {
        delete pIPAdapterInfo;
        pIPAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[IPAdapterInfoSize];
        IPAdapterInfoRealSize = GetAdaptersInfo(pIPAdapterInfo, &IPAdapterInfoSize);
    }

    if (ERROR_SUCCESS == IPAdapterInfoRealSize)
    {
        while (pIPAdapterInfo != NULL && MacAddress.empty())
        {
            IP_ADDR_STRING *pIpAddrString = &(pIPAdapterInfo->IpAddressList);
            while (pIpAddrString != NULL)
            {
                if (strcmp(pIpAddrString->IpAddress.String, BaseIP.c_str()) != 0)
                {
                    pIpAddrString = pIpAddrString->Next;
                    continue;
                }

                std::string TempMacAddress;
                char MacAddressCharArray[512];
                memset(MacAddressCharArray, 0, sizeof(MacAddressCharArray));

                for (UINT i = 0; i < pIPAdapterInfo->AddressLength; i++)
                {
                    sprintf_s(MacAddressCharArray, (i == pIPAdapterInfo->AddressLength - 1) ? "%02x\n" : "%02x-", pIPAdapterInfo->Address[i]);
                    TempMacAddress += MacAddressCharArray;
                }

                std::transform(TempMacAddress.begin(), TempMacAddress.end(), TempMacAddress.begin(), ::toupper);
                MacAddress = TempMacAddress;
                break;
            }

            pIPAdapterInfo = pIPAdapterInfo->Next;
        }
    }

    delete pIPAdapterInfo;
    return !MacAddress.empty();
}

bool LocalComputer::GetProcessElevation(TOKEN_ELEVATION_TYPE * pElevationType, bool &IsAdmin) const
{
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        return false;
    }

    DWORD dwSize = 0;
    BOOL *pIsAdmin = FALSE;
    bool bGetResult = false;

    //Retrieve elevation type information
    if (GetTokenInformation(hToken, TokenElevationType, pElevationType, sizeof(TOKEN_ELEVATION_TYPE), &dwSize))
    {
        //Create the SID corresponding to the administrators group
        BYTE AdminSIDArray[SECURITY_MAX_SID_SIZE];
        dwSize = sizeof(AdminSIDArray);
        CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &AdminSIDArray, &dwSize);

        if (*pElevationType == TokenElevationTypeLimited)
        {
            //Get handle to linked token(will have one if we are lua)
            HANDLE hUnfiltereToken = NULL;
            GetTokenInformation(hToken, TokenLinkedToken, (VOID*)&hUnfiltereToken, sizeof(HANDLE), &dwSize);

            //Check if this original token contains admin SID
            bGetResult = CheckTokenMembership(hUnfiltereToken, &AdminSIDArray, pIsAdmin) != FALSE;
            CloseHandle(hUnfiltereToken);
        }
        else
        {
            *pIsAdmin = IsUserAnAdmin();
            bGetResult = true;
        }

        IsAdmin = (*pIsAdmin != FALSE);
    }

    CloseHandle(hToken);
    return bGetResult;
}

int LocalComputer::GetProcessorsCount() const
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
}

std::string LocalComputer::ConvertToIPString(SOCKADDR_IN * ClientAddr) const
{
    char TempIPArray[INET_ADDRSTRLEN];
    memset(TempIPArray, 0, sizeof(TempIPArray));

    inet_ntop(AF_INET, &(ClientAddr->sin_addr), TempIPArray, sizeof(TempIPArray));
    return std::string(TempIPArray);
}

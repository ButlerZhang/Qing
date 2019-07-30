#pragma once
#include <vector>
#include <string>
#include <WinSock2.h>



class LocalComputer
{
public:

    LocalComputer(void);
    ~LocalComputer(void);

    bool IsProgramExisted(const std::string &ProgramName) const;
    bool IsProgramExisted(const std::wstring &ProgramName) const;

    bool StartProgram(const std::string &ProgramName) const;
    bool StartProgram(const std::wstring &ProgramName) const;
    bool StartProgramEx(const std::wstring &ProgramName) const;
    bool StartTouchScreenCalibration(HWND CurrentHWND) const;

    bool KillProgram(const std::string &ProgramName) const;
    bool KillProgram(const std::wstring &ProgramName) const;

    bool SetOSDateTimeFormat(/*YYYY-MM-DD HH:mm:ss*/) const;
    void SetTaskAndStartMenuVisible(int VisibleValue) const;

    bool GetIPAddress(std::vector<std::string> &IPVector) const;
    bool GetMacAddress(std::string &MacAddress, const std::string &BaseIP) const;
    bool GetProcessElevation(TOKEN_ELEVATION_TYPE *pElevationType, bool &IsAdmin) const;

    int  GetProcessorsCount() const;
    std::string ConvertToIPString(SOCKADDR_IN *ClientAddr) const;
};

#pragma once
#include "QingBase.h"
#include <vector>
#include <string>
#include <Windows.h>

QING_NAMESPACE_BEGIN



class QING_DLL LocalComputer
{
public:

    LocalComputer(void);
    ~LocalComputer(void);

    bool IsProgramExisted(const std::string &ProgramName) const;
    bool IsProgramExisted(const std::wstring &ProgramName) const;

    bool StartProgram(const std::string &ProgramName) const;
    bool StartProgram(const std::wstring &ProgramName) const;
    bool StartTouchScreenCalibration(HWND CurrentHWND) const;

    bool KillProgram(const std::string &ProgramName) const;
    bool KillProgram(const std::wstring &ProgramName) const;

    bool SetOSDateTimeFormat(/*YYYY-MM-DD HH:mm:ss*/) const;
    void SetTaskAndStartMenuVisible(int VisibleValue) const;

    bool GetIPAddress(std::vector<std::string> &IPVector) const;
    bool GetMacAddress(std::string &MacAddress, const std::string &BaseIP) const;

    int  GetProcessorsCount() const;
};

QING_NAMESPACE_END
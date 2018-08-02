#pragma once
#include "QingBase.h"
#include <vector>
#include <string>
#include <Windows.h>



namespace Qing
{
    class QING_DLL LocalComputer
    {
    public:

        LocalComputer(void);
        ~LocalComputer(void);

        bool IsExistProcess(const std::wstring &ProgramName) const;
        void SetTaskAndStartMenuVisible(int VisibleValue) const;

        bool GetMacAddress(std::string &MacAddress, const char* ClientIP) const;
        bool GetLocalIPAddress(std::vector<std::string> &IPVector) const;

        bool KillProgram(const std::wstring &ProgramName) const;
        bool StartProgram(const std::wstring &ProgramName) const;
        int  StartTouchScreenCalibration(HWND CurrentHWND) const;

        BOOL SetOSDateTimeFormat() const;
        std::string TranslateDateTime(const std::string &SourceTime) const;
    };
}

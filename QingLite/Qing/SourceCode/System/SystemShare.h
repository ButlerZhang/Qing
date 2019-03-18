#pragma once
#include "..\QingBase.h"
#include <string>
#include <vector>
#include <Windows.h>

QING_NAMESPACE_BEGIN



std::wstring QING_DLL GetLastErrorString(DWORD LastErrorCode);

std::wstring QING_DLL StringToWString(const std::string &String, int Codepage = CP_UTF8);
std::string  QING_DLL WStringToString(const std::wstring &WString, int Codepage = CP_UTF8);

void QING_DLL SplitCommandLine(std::vector<std::wstring> &CommandLineVector);
void QING_DLL SplitEnvironment(std::vector<std::wstring> &EnvironmentVector);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring QING_DLL GetGUID();
std::wstring QING_DLL GetProgramName();
std::wstring QING_DLL GetRunningPath();

void QING_DLL SetProgramDMPEnable(bool EnableProgramDMP);

QING_NAMESPACE_END
#pragma once
#include <string>
#include <vector>
#include <Windows.h>



std::wstring GetLastErrorString(DWORD LastErrorCode);

std::wstring StringToWString(const std::string &String, int Codepage = CP_UTF8);
std::string  WStringToString(const std::wstring &WString, int Codepage = CP_UTF8);

void SplitCommandLine(std::vector<std::wstring> &CommandLineVector);
void SplitEnvironment(std::vector<std::wstring> &EnvironmentVector);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring GetGUID();
std::wstring GetProgramName();
std::wstring GetRunningPath();

void SetProgramDMPEnable(bool EnableProgramDMP);

#pragma once
#include "QingBase.h"
#include <string>
#include <vector>
#include <Windows.h>

QING_NAMESPACE_BEGIN


std::wstring QING_DLL GetGUID();
std::wstring QING_DLL GetProgramName();
std::wstring QING_DLL GetRunningPath();
std::wstring QING_DLL ConvertDoubleToString(double Value, int precision);

std::string  QING_DLL GetSHA1(const std::string &DataString, bool IsUpper);
std::wstring QING_DLL GetSHA1(const std::wstring &DataString, bool IsUpper);

std::wstring QING_DLL StringToWString(const std::string &String, int Codepage = CP_UTF8);
std::string  QING_DLL WStringToString(const std::wstring &WString, int Codepage = CP_UTF8);

void QING_DLL SplitString(const std::wstring &SourceString, std::vector<std::wstring> &StringVector, const std::wstring &Seperator);

QING_NAMESPACE_END
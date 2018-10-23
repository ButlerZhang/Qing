#pragma once
#include "QingBase.h"
#include <string>
#include <Windows.h>

QING_NAMESPACE_BEGIN


std::wstring QING_DLL GetGUID();
std::wstring QING_DLL GetProgramName();
std::wstring QING_DLL GetRunningPath();

std::string  QING_DLL GetSHA1(const std::string &DataString);
std::wstring QING_DLL GetSHA1(const std::wstring &DataString);

std::wstring QING_DLL StringToWString(const std::string &String, int Codepage = CP_UTF8);
std::string  QING_DLL WStringToString(const std::wstring &WString, int Codepage = CP_UTF8);

QING_NAMESPACE_END
#pragma once
#include "QingBase.h"
#include <string>
#include <Windows.h>

QING_NAMESPACE_BEGIN


std::string QING_DLL GetGUID();

std::wstring QING_DLL StringToWString(const std::string &str, int codepage = CP_UTF8);
std::string  QING_DLL WStringToString(const std::wstring &wstr, int codepage = CP_UTF8);

QING_NAMESPACE_END
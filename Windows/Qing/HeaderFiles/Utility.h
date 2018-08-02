#pragma once
#include "QingBase.h"
#include <string>
#include <Windows.h>



namespace Qing
{
    std::wstring QING_DLL StringToWString(const std::string &str, int codepage = CP_UTF8);
    std::string  QING_DLL WStringToString(const std::wstring &wstr, int codepage = CP_UTF8);
}
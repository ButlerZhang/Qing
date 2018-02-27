#pragma once

#include <string>
#include <Windows.h>



namespace QING
{
    std::wstring StringToWString(const std::string &str, int codepage = CP_UTF8);
    std::string  WStringToString(const std::wstring &wstr, int codepage = CP_UTF8);
}

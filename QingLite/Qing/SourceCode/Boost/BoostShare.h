#pragma once
#include "..\QingBase.h"
#include <string>

QING_NAMESPACE_BEGIN

bool Base64Encode(const std::wstring &Input, std::wstring &Output);
bool Base64Decode(const std::wstring &Input, std::wstring &Output);

std::string  QING_DLL GetSHA1(const std::string &DataString, bool IsUpper);
std::wstring QING_DLL GetSHA1(const std::wstring &DataString, bool IsUpper);

QING_NAMESPACE_END
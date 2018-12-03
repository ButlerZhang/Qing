#pragma once
#include "..\QingBase.h"
#include <string>
#include <vector>

QING_NAMESPACE_BEGIN



std::wstring QING_DLL ConvertDoubleToString(double Value, int precision);
void QING_DLL SplitString(const std::wstring &SourceString, std::vector<std::wstring> &StringVector, const std::wstring &Seperator);

QING_NAMESPACE_END
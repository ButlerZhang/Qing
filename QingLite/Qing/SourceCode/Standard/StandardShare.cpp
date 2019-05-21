#include "StandardShare.h"
#include <sstream>

QING_NAMESPACE_BEGIN



std::wstring ConvertDoubleToString(double Value, int precision)
{
    std::wostringstream ConvertString;

    ConvertString.precision(precision);
    ConvertString.setf(std::ios::fixed);
    ConvertString << Value;

    return ConvertString.str();
}

void SplitString(const std::wstring & SourceString, std::vector<std::wstring>& StringVector, const std::wstring & Seperator)
{
    std::wstring::size_type pos1 = 0;
    std::wstring::size_type pos2 = SourceString.find(Seperator);

    while (std::wstring::npos != pos2)
    {
        StringVector.push_back(SourceString.substr(pos1, pos2 - pos1));

        pos1 = pos2 + Seperator.size();
        pos2 = SourceString.find(Seperator, pos1);
    }

    if (pos1 != SourceString.length())
    {
        StringVector.push_back(SourceString.substr(pos1));
    }
}

QING_NAMESPACE_END
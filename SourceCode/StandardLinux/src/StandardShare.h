#pragma once
#include <string>
#include <vector>
#include <sstream>



inline std::string ConvertDoubleToString(double Value, int precision)
{
    std::ostringstream ConvertString;

    ConvertString.precision(precision);
    ConvertString.setf(std::ios::fixed);
    ConvertString << Value;

    return ConvertString.str();
}

inline bool SplitString(const std::string &SourceString, std::vector<std::string> &StringVector, const std::string &Seperator)
{
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = SourceString.find(Seperator);

    while (std::string::npos != pos2)
    {
        const std::string &TempString = SourceString.substr(pos1, pos2 - pos1);
        if (!TempString.empty())
        {
            StringVector.push_back(TempString);
        }

        pos1 = pos2 + Seperator.size();
        pos2 = SourceString.find(Seperator, pos1);
    }

    if (pos1 != SourceString.length())
    {
        StringVector.push_back(SourceString.substr(pos1));
    }

    return !StringVector.empty();
}

inline bool SplitString(const std::wstring &SourceString, std::vector<std::wstring> &StringVector, const std::wstring &Seperator)
{
    std::wstring::size_type pos1 = 0;
    std::wstring::size_type pos2 = SourceString.find(Seperator);

    while (std::wstring::npos != pos2)
    {
        const std::wstring &TempString = SourceString.substr(pos1, pos2 - pos1);
        if (!TempString.empty())
        {
            StringVector.push_back(TempString);
        }

        pos1 = pos2 + Seperator.size();
        pos2 = SourceString.find(Seperator, pos1);
    }

    if (pos1 != SourceString.length())
    {
        StringVector.push_back(SourceString.substr(pos1));
    }

    return !StringVector.empty();
}
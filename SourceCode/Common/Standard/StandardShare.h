#pragma once
#include <string>
#include <vector>
#include <sstream>



std::string ConvertDoubleToString(double Value, int precision)
{
    std::ostringstream ConvertString;

    ConvertString.precision(precision);
    ConvertString.setf(std::ios::fixed);
    ConvertString << Value;

    return ConvertString.str();
}

bool SplitString(const std::string &SourceString, std::vector<std::string> &StringVector, const std::string &Seperator)
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

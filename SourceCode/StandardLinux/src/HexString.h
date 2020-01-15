#pragma once
#include <string>



class HexString
{
public:

    HexString(void)
    {
    }

    ~HexString(void)
    {
    }

    int HexCharToInt(char Ch) const
    {
        if (Ch >= '0' && Ch <= '9') return (Ch - '0');
        if (Ch >= 'A' && Ch <= 'F') return (Ch - 'A' + 10);
        if (Ch >= 'a' && Ch <= 'f') return (Ch - 'a' + 10);

        return 0;
    }

    std::string AddSpace(const std::string &SourcesString, int Interval) const
    {
        std::string Space(" ");
        std::string TempString = SourcesString;

        for (std::string::size_type Index = Interval; Index < TempString.size(); Index += Interval)
        {
            TempString.insert(Index, Space);
            ++Index;
        }

        return TempString;
    }

    void HexStringToASCIIString(const std::string &MyHexString, std::string &ASCIIString) const
    {
        ASCIIString.clear();
        for (std::string::size_type Index = 0; Index < MyHexString.size(); Index += 2)
        {
            ASCIIString += static_cast<char>(((HexCharToInt(MyHexString[Index]) << 4) | HexCharToInt(MyHexString[Index + 1])));
        }
    }

    void ASCIIStringToHexString(const std::string &ASCIIString, std::string &HexStringResult, bool IsLowerCase) const
    {
        HexStringResult.clear();
        std::string FlagString(IsLowerCase ? "0123456789abcdef" : "0123456789ABCDEF");

        int b = 0;
        for (std::string::size_type i = 0; i < ASCIIString.size(); i++)
        {
            b = 0x0f & (ASCIIString[i] >> 4);
            HexStringResult.append(1, FlagString.at(b));

            b = 0x0f & ASCIIString[i];
            HexStringResult.append(1, FlagString.at(b));
        }
    }

    std::string ASCIIStringToHexString(const char *ASCIIString, ssize_t ASCIISize, bool IsLowerCase) const
    {
        std::string HexStringResult;
        std::string FlagString(IsLowerCase ? "0123456789abcdef" : "0123456789ABCDEF");

        int b = 0;
        for (ssize_t i = 0; i < ASCIISize; i++)
        {
            b = 0x0f & (ASCIIString[i] >> 4);
            HexStringResult.append(1, FlagString.at(b));

            b = 0x0f & ASCIIString[i];
            HexStringResult.append(1, FlagString.at(b));
        }

        return HexStringResult;
    }
};

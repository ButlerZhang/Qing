#pragma once
#include <string>



class HexString
{
public:

    HexString(void);
    ~HexString(void);

    int HexCharToInt(char ch) const;
    std::string AddSpace(const std::string &SourcesString, int Interval) const;

    void HexStringToASCIIString(const std::string &MyHexString, std::string &ASCIIString) const;
    void ASCIIStringToHexString(const std::string &ASCIIString, std::string &HexStringResult, bool IsLowerCase) const;
};

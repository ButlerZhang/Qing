#pragma once
#include "..\QingBase.h"
#include <string>

QING_NAMESPACE_BEGIN



class QING_DLL NumericalConversion
{
public:

    NumericalConversion(void);
    ~NumericalConversion(void);

    bool IsHexString(const std::string &HexString) const;
    bool IsDecString(const std::string &DecString) const;
    bool IsBinString(const std::string &BinString) const;

    bool DecToBin(const std::string &DecString, std::string &BinString) const;
    bool BinToDec(const std::string &BinString, std::string &DecString) const;

    bool HexToBin(const std::string &HexString, std::string &BinString) const;
    bool BinToHex(const std::string &BinString, std::string &HexString) const;

    bool HexToDec(const std::string &HexString, std::string &DecString) const;
    bool DecToHex(const std::string &DecString, std::string &HexString) const;

private:

    std::size_t GetNotZeroStartPosition(const std::string &SourceString) const;

    void Add(std::string &BasicString, std::string &AddString) const;
};

QING_NAMESPACE_END
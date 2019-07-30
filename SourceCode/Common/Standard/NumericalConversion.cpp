#include "NumericalConversion.h"
#include <algorithm>



NumericalConversion::NumericalConversion(void)
{
}

NumericalConversion::~NumericalConversion(void)
{
}

bool NumericalConversion::IsHexString(const std::string &HexString) const
{
    for (std::size_t Index = 0; Index < HexString.size(); Index++)
    {
        if (HexString[Index] >= '0' && HexString[Index] <= '9')  continue;
        if (HexString[Index] >= 'A' && HexString[Index] <= 'F')  continue;
        if (HexString[Index] >= 'a' && HexString[Index] <= 'f')  continue;

        return false;
    }

    return true;
}

bool NumericalConversion::IsDecString(const std::string &DecString) const
{
    for (std::size_t Index = 0; Index < DecString.size(); Index++)
    {
        if (!isdigit(DecString[Index]))
        {
            return false;
        }
    }

    return true;
}

bool NumericalConversion::IsBinString(const std::string &BinString) const
{
    for (std::size_t Index = 0; Index < BinString.size(); Index++)
    {
        if (BinString[Index] != '0' && BinString[Index] != '1')
        {
            return false;
        }
    }

    return true;
}

bool NumericalConversion::DecToBin(const std::string &DecString, std::string &BinString) const
{
    if (DecString.empty() || !IsDecString(DecString))
    {
        return false;
    }

    int Value = 0;
    int Remainder = 0;
    std::string TempDecString(DecString);
    std::size_t NotZeroStartIndex = GetNotZeroStartPosition(TempDecString);

    while (NotZeroStartIndex < TempDecString.size())
    {
        for (std::size_t Index = NotZeroStartIndex; Index < TempDecString.size(); Index++)
        {
            Value = TempDecString[Index] - '0' + (Remainder == 0 ? 0 : 10);
            Remainder = Value % 2;
            Value = Value / 2;

            TempDecString[Index] = static_cast<char>(Value + 48);
        }

        BinString += (Remainder == 0 ? '0' : '1');
        Remainder = 0;

        if (TempDecString[NotZeroStartIndex] == '0')
        {
            ++NotZeroStartIndex;
        }
    }

    std::reverse(BinString.begin(), BinString.end());
    return true;
}

bool NumericalConversion::BinToDec(const std::string &BinString, std::string &DecString) const
{
    if (BinString.empty() || !IsBinString(BinString))
    {
        return false;
    }

    std::size_t NotZeroStartIndex = GetNotZeroStartPosition(BinString);
    DecString = BinString[NotZeroStartIndex];
    std::string AddString;

    for (std::size_t Index = NotZeroStartIndex; Index < BinString.size() - 1; Index++)
    {
        //algorithm : HexString = HexString * 2 + BinString[Index+1];

        Add(DecString, DecString);
        if (BinString[Index + 1] != '0')
        {
            AddString.resize(DecString.size(), '0');
            AddString[0] = BinString[Index + 1];
            Add(DecString, AddString);
        }
    }

    std::reverse(DecString.begin(), DecString.end());
    return true;
}

bool NumericalConversion::HexToBin(const std::string &HexString, std::string &BinString) const
{
    if (HexString.empty() || !IsHexString(HexString))
    {
        return false;
    }

    int ArrayIndex = 0;
    std::string DecToBinHashArray[] = { "0000","0001","0010","0011","0100","0101","0110","0111","1000","1001","1010","1011","1100","1101","1110","1111" };

    for (std::size_t Index = 0; Index < HexString.size(); Index++)
    {
        if (HexString[Index] >= '0' && HexString[Index] <= '9')
        {
            ArrayIndex = HexString[Index] - '0';
        }
        else if (HexString[Index] >= 'A' && HexString[Index] <= 'F')
        {
            ArrayIndex = HexString[Index] - 'A' + 10;
        }
        else
        {
            ArrayIndex = HexString[Index] - 'a' + 10;
        }

        BinString += DecToBinHashArray[ArrayIndex];
    }

    return true;
}

bool NumericalConversion::BinToHex(const std::string &BinString, std::string &HexString) const
{
    if (BinString.empty() || !IsBinString(BinString))
    {
        return false;
    }

    std::string TempBinString(BinString);
    while (TempBinString.size() % 4 != 0)
    {
        TempBinString.insert(0, "0");
    }

    for (std::size_t Index = 0; Index < TempBinString.size(); Index += 4)
    {
        int Value = 0;
        Value += TempBinString[Index] == '0' ? 0 : 8;
        Value += TempBinString[Index + 1] == '0' ? 0 : 4;
        Value += TempBinString[Index + 2] == '0' ? 0 : 2;
        Value += TempBinString[Index + 3] == '0' ? 0 : 1;

        switch (Value)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:     HexString += static_cast<char>(Value + 48);         break;
        case 10:    HexString += 'A';                                   break;
        case 11:    HexString += 'B';                                   break;
        case 12:    HexString += 'C';                                   break;
        case 13:    HexString += 'D';                                   break;
        case 14:    HexString += 'E';                                   break;
        case 15:    HexString += 'F';                                   break;
        }
    }

    return true;
}

bool NumericalConversion::HexToDec(const std::string &HexString, std::string &DecString) const
{
    std::string BinString;
    if (HexToBin(HexString, BinString))
    {
        return BinToDec(BinString, DecString);
    }

    return false;
}

bool NumericalConversion::DecToHex(const std::string &DecString, std::string &HexString) const
{
    std::string BinString;
    if (DecToBin(DecString, BinString))
    {
        return BinToHex(BinString, HexString);
    }

    return false;
}

std::size_t NumericalConversion::GetNotZeroStartPosition(const std::string &SourceString) const
{
    std::size_t NotZeroStartPosition = 0;
    while (SourceString[NotZeroStartPosition] == '0')
    {
        ++NotZeroStartPosition;
    }

    return NotZeroStartPosition;
}

void NumericalConversion::Add(std::string &BasicString, std::string &AddString) const
{
    while (BasicString.size() < AddString.size())
    {
        BasicString += '0';
    }

    while (AddString.size() < BasicString.size())
    {
        AddString += '0';
    }

    int Value = 0;
    int Carray = 0;

    for (std::size_t Index = 0; Index < BasicString.size(); Index++)
    {
        Value = (BasicString[Index] - '0') + (AddString[Index] - '0') + Carray;
        if (Value >= 10)
        {
            Carray = 1;
            Value -= 10;
        }
        else
        {
            Carray = 0;
        }

        BasicString[Index] = static_cast<char>(Value + 48);
    }

    if (Carray == 1)
    {
        BasicString += '1';
    }
}

#pragma once
#include <openssl/sha.h>
#include "../../../../Common/Tool/HexString.h"



std::string GetSHA256String(const std::string &SourceString)
{
    const int CHAR_SIZE = 32;
    unsigned char SHA256String[CHAR_SIZE + 1];
    memset(SHA256String, 0, sizeof(SHA256String));

    SHA256((const unsigned char *)SourceString.c_str(), SourceString.length(), SHA256String);
    return std::string((const char*)SHA256String, CHAR_SIZE);
}

std::string GetSHA256HexString(const std::string &SourceString, bool IsLowerCase = false)
{
    HexString MyHexString;
    std::string SHA256HexString;
    MyHexString.ASCIIStringToHexString(GetSHA256String(SourceString), SHA256HexString, IsLowerCase);

    return SHA256HexString;
}

void GetSHA256(const std::string &SourceString, std::string &SHA256String, std::string &SHA256HexString, bool IsLowerCase = false)
{
    HexString MyHexString;
    SHA256String = GetSHA256String(SourceString);
    MyHexString.ASCIIStringToHexString(SHA256String, SHA256HexString, IsLowerCase);
}

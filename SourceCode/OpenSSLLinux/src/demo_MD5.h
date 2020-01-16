#pragma once
#include <string.h>
#include <openssl/md5.h>
#include "../../StandardLinux/src/HexString.h"



std::string GetMD5String(const std::string &SourceString)
{
    const int CHAR_SIZE = 32;
    unsigned char MD5String[CHAR_SIZE + 1];
    memset(MD5String, 0, sizeof(MD5String));

    MD5((const unsigned char *)SourceString.c_str(), SourceString.length(), MD5String);
    return std::string((const char *)MD5String, CHAR_SIZE);
}

std::string GetMD5HexString(const std::string &SourceString, bool IsLowerCase = false)
{
    HexString MyHexString;
    std::string MD5HexString;
    MyHexString.ASCIIStringToHexString(GetMD5String(SourceString), MD5HexString, IsLowerCase);

    return MD5HexString;
}

void GetMD5(const std::string &SourceString, std::string &MD5String, std::string &MD5HexString, bool IsLowerCase = false)
{
    HexString MyHexString;
    MD5String = GetMD5String(SourceString);
    MyHexString.ASCIIStringToHexString(MD5String, MD5HexString, IsLowerCase);
}

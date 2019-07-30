#pragma once
#include <string>



bool Base64Encode(const std::string &Input, std::string &Output);
bool Base64Decode(const std::string &Input, std::string &Output);

std::string  GetSHA1(const std::string &DataString, bool IsUpper);
std::wstring GetSHA1(const std::wstring & DataString, bool IsUpper);
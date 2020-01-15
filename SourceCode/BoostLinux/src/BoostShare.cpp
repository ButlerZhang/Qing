#include "BoostShare.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
//#include <boost\uuid\name_generator_sha1.hpp>
//#include <boost\beast\core\detail\base64.hpp>


bool Base64Encode(const std::string &Input, std::string &Output)
{
    //Output = boost::beast::detail::base64_encode(Input);
    //add
    return !Output.empty();
}

bool Base64Decode(const std::string &Input, std::string &Output)
{
    //Output = boost::beast::detail::base64_decode(Input);
    //add
    return !Output.empty();
}

bool Base64Encode(const std::wstring &Input, std::wstring &Output)
{
    return false;
}

bool Base64Decode(const std::wstring &Input, std::wstring &Output)
{
    return false;
}

std::string GetSHA1(const std::string & DataString, bool IsUpper)
{
    //boost::uuids::detail::sha1 sha;
    //sha.process_bytes(DataString.c_str(), DataString.size());

    const int DigestArraySize = 5;
    unsigned int DigestArray[DigestArraySize];
    //sha.get_digest(DigestArray);

    std::ostringstream ConvertBuffer;
    for (int Index = 0; Index < DigestArraySize; ++Index)
    {
        ConvertBuffer << std::hex << std::setfill('0') << std::setw(8) << DigestArray[Index];
    }

    if (IsUpper)
    {
        std::string ResultString(ConvertBuffer.str());
        std::transform(ResultString.begin(), ResultString.end(), ResultString.begin(), ::toupper);
        return ResultString;
    }

    return std::string(ConvertBuffer.str());
}

std::wstring GetSHA1(const std::wstring & DataString, bool IsUpper)
{
    //boost::uuids::detail::sha1 sha;
    //sha.process_bytes(DataString.c_str(), DataString.size());

    const int DigestArraySize = 5;
    unsigned int DigestArray[DigestArraySize];
    //sha.get_digest(DigestArray);

    std::wostringstream ConvertBuffer;
    for (int Index = 0; Index < DigestArraySize; ++Index)
    {
        ConvertBuffer << std::hex << std::setfill(L'0') << std::setw(8) << DigestArray[Index];
    }

    if (IsUpper)
    {
        std::wstring ResultString(ConvertBuffer.str());
        std::transform(ResultString.begin(), ResultString.end(), ResultString.begin(), ::toupper);
        return ResultString;
    }

    return std::wstring(ConvertBuffer.str());
}

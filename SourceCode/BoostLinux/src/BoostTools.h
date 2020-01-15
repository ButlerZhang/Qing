#include "BoostLog.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/uuid/name_generator_sha1.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/property_tree/ptree.hpp>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>


bool Base64Encode(const std::string &Input, std::string &Output)
{
    Output = boost::beast::detail::base64_encode(Input);
    return !Output.empty();
}

bool Base64Decode(const std::string &Input, std::string &Output)
{
    Output = boost::beast::detail::base64_decode(Input);
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

std::string GetSHA1(const std::string &DataString, bool IsUpper)
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



inline bool IsIPValid(const std::string &IP)
{
    using namespace boost::xpressive;
    cregex reg_ip = cregex::compile("(25[0-4]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[1-9])[.](25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[.](25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[.](25[0-4]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[1-9])");
    return regex_match(IP.c_str(), reg_ip);
}

inline bool GetJsonTree(const std::string &JsonString, boost::property_tree::ptree &JsonTree)
{
    if (JsonString.empty())
    {
        g_Log.WriteError("Get json tree error, json string is empty.");
        return false;
    }

    try
    {
        std::stringstream JsonStream(JsonString);
        boost::property_tree::read_json(JsonStream, JsonTree);
    }
    catch (...)
    {
        g_Log.WriteError(BoostFormat("Get json tree throw error, json string = %s", JsonString.c_str()));
        return false;
    }

    return true;
}

inline std::string GetJsonString(boost::property_tree::ptree &JsonTree)
{
    std::string JsonString;

    try
    {
        std::stringstream JsonStream;
        boost::property_tree::write_json(JsonStream, JsonTree, false);

        JsonString = JsonStream.str();
        JsonString.erase(JsonString.end() - 1);

        g_Log.WriteCore(BoostFormat("Get json string = %s", JsonString.c_str()));
    }
    catch (...)
    {
        g_Log.WriteError("Get json string throw error.");
    }

    return JsonString;
}

inline std::string GetUUID()
{
    boost::uuids::uuid NewUUID = boost::uuids::random_generator()();
    return boost::uuids::to_string(NewUUID);
}

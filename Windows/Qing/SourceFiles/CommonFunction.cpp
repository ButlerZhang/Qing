#include "..\HeaderFiles\CommonFunction.h"
#include "..\HeaderFiles\BoostLog.h"

#include <boost\uuid\name_generator_sha1.hpp>
#include <sstream>
#include <iomanip>

QING_NAMESPACE_BEGIN



std::wstring GetGUID()
{
    GUID guid;
    char Data[64];
    memset(Data, 0, sizeof(Data));

    ::CoInitialize(0);
    if (::CoCreateGuid(&guid) == S_OK)
    {
        _snprintf_s(Data,
            sizeof(Data) - 1,
            "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
            guid.Data1,
            guid.Data2,
            guid.Data3,
            guid.Data4[0],
            guid.Data4[1],
            guid.Data4[2],
            guid.Data4[3],
            guid.Data4[4],
            guid.Data4[5],
            guid.Data4[6],
            guid.Data4[7]);
    }
    ::CoUninitialize();

    return StringToWString(std::string(Data));
}

std::wstring GetProgramName()
{
    wchar_t FullPathCharArray[MAX_PATH];
    memset(FullPathCharArray, 0, sizeof(FullPathCharArray));
    ::GetModuleFileName(NULL, FullPathCharArray, MAX_PATH);

    std::wstring FullPathString(FullPathCharArray);
    if (!FullPathString.empty())
    {
        std::size_t StartIndex = FullPathString.find_last_of('\\') + 1;
        if (StartIndex != std::wstring::npos)
        {
            std::size_t CharCount = FullPathString.size() - StartIndex - 4; //sizeof(.exe) == 4
            std::wstring ProgramName = FullPathString.substr(StartIndex, CharCount);
            return ProgramName;
        }
    }

    BoostLog::WriteError(L"Return wrong program name.");
    return L"Qing";
}

std::wstring GetRunningPath()
{
    wchar_t FullPathCharArray[MAX_PATH];
    memset(FullPathCharArray, 0, sizeof(FullPathCharArray));
    ::GetModuleFileName(NULL, FullPathCharArray, MAX_PATH);

    std::wstring FullPathString(FullPathCharArray);
    if (!FullPathString.empty())
    {
        std::size_t StopIndex = FullPathString.find_last_of('\\') + 1;
        if (StopIndex != std::wstring::npos)
        {
            return FullPathString.substr(0, StopIndex);
        }
    }

    BoostLog::WriteError(L"Return wrong running path.");
    return L"Qing";
}

std::string QING_DLL GetSHA1(const std::string & DataString)
{
    boost::uuids::detail::sha1 sha;
    sha.process_bytes(DataString.c_str(), DataString.size());

    const int DigestArraySize = 5;
    unsigned int DigestArray[DigestArraySize];
    sha.get_digest(DigestArray);

    std::ostringstream ConvertBuffer;
    for (int Index = 0; Index < DigestArraySize; ++Index)
    {
        ConvertBuffer << std::hex << std::setfill('0') << std::setw(8) << DigestArray[Index];
    }

    return std::string(ConvertBuffer.str());
}

std::wstring QING_DLL GetSHA1(const std::wstring & DataString)
{
    boost::uuids::detail::sha1 sha;
    sha.process_bytes(DataString.c_str(), DataString.size());

    const int DigestArraySize = 5;
    unsigned int DigestArray[DigestArraySize];
    sha.get_digest(DigestArray);

    std::wostringstream ConvertBuffer;
    for (int Index = 0; Index < DigestArraySize; ++Index)
    {
        ConvertBuffer << std::hex << std::setfill(L'0') << std::setw(8) << DigestArray[Index];
    }

    return std::wstring(ConvertBuffer.str());
}

std::wstring StringToWString(const std::string &String, int Codepage)
{
    int Length = MultiByteToWideChar(Codepage, 0, (LPCSTR)String.c_str(), static_cast<int>(String.length()), NULL, 0);

    std::wstring ResultWString(Length, L'\0');
    int ResultLength = MultiByteToWideChar(Codepage, 0, (LPCSTR)String.c_str(), static_cast<int>(String.length()), (LPWSTR)ResultWString.c_str(), Length);

    return ResultWString;
}

std::string WStringToString(const std::wstring &WString, int Codepage)
{
    int SourceLength = static_cast<int>(WString.length()) + 1;
    int TargetLength = WideCharToMultiByte(Codepage, 0, (LPCWSTR)WString.c_str(), SourceLength, NULL, 0, NULL, FALSE) + 1;

    std::string ResultString(TargetLength, '\0');
    int ResultLength = WideCharToMultiByte(Codepage, 0, (LPCWSTR)WString.c_str(), SourceLength, (LPSTR)ResultString.c_str(), TargetLength, NULL, FALSE);

    return ResultString;
}

QING_NAMESPACE_END